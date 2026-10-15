/**
 * @file join_query_tests.cpp
 * @brief Comprehensive tests for equi-join queries — with and without WHERE clauses.
 *
 * Data layout after JoinOperator replication (N=left rows, M=right rows):
 *
 *   Slot index = j * N + i
 *     i in [0, N-1]  -> left table row index
 *     j in [0, M-1]  -> right table row index
 *
 * All encryption and homomorphic evaluation use real BFV ciphertexts.
 * MockScanOperator only replaces disk I/O — the crypto path is fully live.
 *
 * Test matrix
 * ───────────
 *  Planner
 *    1. PlanJoinQuerySuccessfully
 *    2. PlanJoinWithWhereClause
 *    3. PlanJoinThrowsOnNonExistentTable
 *
 *  JoinOperator — mask correctness
 *    4. JoinMaskTwoByTwo_PartialMatch          (2x2, 2 matching pairs)
 *    5. JoinMaskAllZeroWhenNoRowsMatch          (2x2, no matches)
 *    6. JoinMaskOneByOne_MatchingRows           (1x1, single match)
 *    7. JoinBatchHasQualifiedAndUnqualifiedCols (column naming)
 *    8. JoinRowCountIsNTimesM                   (row count arithmetic)
 *
 *  ExecutionEngine — join + WHERE combined mask
 *    9.  JoinWhereOnLeftColumn   (grade > threshold, keeps 1 of 2 matches)
 *   10.  JoinWhereOnRightColumn  (score > threshold, keeps 1 of 2 matches)
 *   11.  JoinWhereEliminatesAll  (impossible threshold, combined mask all-zero)
 *   12.  JoinWherePreservesAll   (trivially-true threshold, combined = join mask)
 *   13.  JoinAndWhereCompound    (grade > t AND score > s, no surviving pair)
 */

#include <gtest/gtest.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/execution/ExecutionEngine.h>
#include <hegadb/server/execution/ExecutionPlan.h>
#include <hegadb/server/operators/FilterOperator.h>
#include <hegadb/server/operators/JoinOperator.h>
#include <hegadb/server/operators/ProjectionOperator.h>
#include <hegadb/server/planner/ExpressionPlanner.h>
#include <hegadb/server/planner/QueryPlanner.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>
#include "query.pb.h"

#include <memory>
#include <vector>
#include <unordered_map>

using namespace hegadb;
using namespace hegadb::server;
using namespace hegadb::server::execution;
using namespace hegadb::server::operators;
using namespace hegadb::server::planner;
using namespace hegadb::catalog;
using namespace hegadb::protocol;
using namespace hegadb::types;

// ─────────────────────────────────────────────────────────────────────────────
// Test infrastructure
// ─────────────────────────────────────────────────────────────────────────────

namespace {

/// Stub StorageManager — never called in operator-level tests.
class DummyStorageManager : public server::storage::StorageManager {
public:
    std::unique_ptr<server::storage::EncryptedTable> openTable(
        std::string const& table_name) override
    {
        throw std::runtime_error("DummyStorageManager: not implemented: " + table_name);
    }
};

/// Injects a pre-built ExecutionBatch into the operator pipeline.
class MockScanOperator : public Operator {
public:
    explicit MockScanOperator(ExecutionBatch batch)
        : batch_(std::move(batch)) {}

    void open(ExecutionContext&) override { returned_ = false; }

    std::optional<ExecutionBatch> next() override
    {
        if (returned_) return std::nullopt;
        returned_ = true;
        return batch_;
    }

    void close() override {}

private:
    ExecutionBatch batch_;
    bool returned_ = false;
};

} // anonymous namespace

// ─────────────────────────────────────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────────────────────────────────────

class JoinQueryTest : public ::testing::Test {
protected:
    // Tables:
    //   students    : id INTEGER, grade INTEGER
    //   enrollments : student_id INTEGER, score INTEGER
    void SetUp() override
    {
        std::vector<ColumnSchema> student_cols = {
            { "id",    DataType::Integer, false, 0 },
            { "grade", DataType::Integer, false, 1 },
        };
        std::vector<ColumnSchema> enrollment_cols = {
            { "student_id", DataType::Integer, false, 0 },
            { "score",      DataType::Integer, false, 1 },
        };

        std::unordered_map<std::string, TableSchema> tables;
        tables.emplace("students",    TableSchema("students",    student_cols));
        tables.emplace("enrollments", TableSchema("enrollments", enrollment_cols));

        auto snapshot = std::make_shared<CatalogSnapshot const>(1, std::move(tables));
        catalog_  = std::make_shared<InMemoryCatalog>(snapshot);
        storage_  = std::make_shared<DummyStorageManager>();

        crypto_context_ = std::make_shared<crypto::OpenFHEContext>();
        crypto_context_->enableFeatures();

        auto bfv = bfvCtx();
        keypair_ = bfv->KeyGen();
        bfv->EvalMultKeyGen(keypair_.secretKey);

        // Rotation keys covering all shifts used by replicateLeft/Right
        // and by the comparison polynomial evaluator.
        std::vector<int32_t> rotIdx;
        for (int32_t i = 1; i <= 32; ++i) {
            rotIdx.push_back(i);
            rotIdx.push_back(-i);
        }
        int32_t rd = static_cast<int32_t>(bfv->GetRingDimension());
        for (int32_t s = 64; s < rd / 2; s *= 2) {
            rotIdx.push_back(s);
            rotIdx.push_back(-s);
        }
        bfv->EvalRotateKeyGen(keypair_.secretKey, rotIdx);
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> bfvCtx() const
    {
        return crypto_context_->getBFVContext();
    }

    size_t ringDim() const { return bfvCtx()->GetRingDimension(); }

    /// Encrypt `data` (zero-padded to ring_dim) as a BFV Integer ciphertext.
    crypto::OpenFHECiphertext encryptColumn(std::vector<int64_t> const& data) const
    {
        std::vector<int64_t> vec(ringDim(), 0);
        for (size_t i = 0; i < data.size(); ++i) vec[i] = data[i];
        auto pt = bfvCtx()->MakePackedPlaintext(vec);
        auto ct = bfvCtx()->Encrypt(keypair_.publicKey, pt);
        return { ct, crypto::SchemeType::BFV, DataType::Integer };
    }

    /// Encrypt a scalar threshold broadcast to every slot (for WHERE parameters).
    /// Broadcasting is required so that slot-wise comparisons across the N*M
    /// replicated batch all use the same threshold value.
    std::string serializeParam(int64_t threshold) const
    {
        std::vector<int64_t> vec(ringDim(), threshold);
        auto pt = bfvCtx()->MakePackedPlaintext(vec);
        auto ct = bfvCtx()->Encrypt(keypair_.publicKey, pt);
        crypto::OpenFHECiphertext wrapper(ct, crypto::SchemeType::BFV, DataType::Integer);
        return wrapper.serialize();
    }

    ExecutionBatch makeStudentBatch(
        std::vector<int64_t> const& id_vals,
        std::vector<int64_t> const& grade_vals) const
    {
        ExecutionBatch b;
        b.setRowCount(id_vals.size());
        b.addColumn("id",    encryptColumn(id_vals));
        b.addColumn("grade", encryptColumn(grade_vals));
        return b;
    }

    ExecutionBatch makeEnrollmentBatch(
        std::vector<int64_t> const& student_id_vals,
        std::vector<int64_t> const& score_vals) const
    {
        ExecutionBatch b;
        b.setRowCount(student_id_vals.size());
        b.addColumn("student_id", encryptColumn(student_id_vals));
        b.addColumn("score",      encryptColumn(score_vals));
        return b;
    }

    /// Canonical join condition: students.id = enrollments.student_id
    ExprPtr makeJoinCondition() const
    {
        return std::make_shared<Expr>(planner::ComparisonExpr{
            std::make_shared<Expr>(planner::ColumnRefExpr{ "students",    "id"         }),
            operations::ComparisonOp::EQ,
            std::make_shared<Expr>(planner::ColumnRefExpr{ "enrollments", "student_id" }),
        });
    }

    /// WHERE <col_name> <op> param[param_idx]
    ExprPtr makeWhere(
        std::string const& col_name,
        operations::ComparisonOp op,
        uint32_t param_idx = 0) const
    {
        return std::make_shared<Expr>(planner::ComparisonExpr{
            std::make_shared<Expr>(planner::ColumnRefExpr{ "", col_name }),
            op,
            std::make_shared<Expr>(planner::ParameterRefExpr{ param_idx }),
        });
    }

    /// left_pred AND right_pred
    ExprPtr makeAndWhere(ExprPtr left_pred, ExprPtr right_pred) const
    {
        return std::make_shared<Expr>(planner::LogicalExpr{
            std::move(left_pred),
            operations::LogicalOp::AND,
            std::move(right_pred),
        });
    }

    /// Build and run a Projection -> [Filter] -> Join plan.
    /// Returns the first N*M slots of the decrypted combined mask.
    ///
    /// @param where_pred  nullptr to omit the FilterOperator
    /// @param params      list of (parameter_index, plaintext_threshold) pairs
    std::vector<int64_t> runJoinQuery(
        ExecutionBatch left_batch,
        ExecutionBatch right_batch,
        ExprPtr where_pred,
        std::vector<std::pair<uint32_t, int64_t>> const& params = {})
    {
        size_t N = left_batch.getRowCount();
        size_t M = right_batch.getRowCount();

        std::unique_ptr<Operator> tree =
            std::make_unique<JoinOperator>(
                std::make_unique<MockScanOperator>(std::move(left_batch)),
                std::make_unique<MockScanOperator>(std::move(right_batch)),
                "students", "enrollments",
                makeJoinCondition());

        if (where_pred) {
            tree = std::make_unique<FilterOperator>(std::move(tree), where_pred);
        }

        // Empty column list = keep all (wildcard projection)
        tree = std::make_unique<ProjectionOperator>(
            std::move(tree), std::vector<std::string>{});

        ExecutionPlan plan;
        plan.setRoot(std::move(tree));

        ExecutionContext context(
            catalog_, storage_, crypto_context_, RESULT_MODE_ROWS_AND_MASK);

        for (auto const& [idx, val] : params) {
            context.getParameters().addParameter(idx, serializeParam(val));
        }

        ExecutionEngine engine;
        auto result = engine.execute(plan, context);

        EXPECT_TRUE(result.hasMask());

        lbcrypto::Plaintext pt;
        bfvCtx()->Decrypt(
            keypair_.secretKey,
            result.getEncryptedMask().getCiphertext().getCiphertext(),
            &pt);

        auto raw = pt->GetPackedValue();
        return { raw.begin(), raw.begin() + static_cast<ptrdiff_t>(N * M) };
    }

    std::shared_ptr<Catalog>                 catalog_;
    std::shared_ptr<crypto::OpenFHEContext>  crypto_context_;
    std::shared_ptr<storage::StorageManager> storage_;
    lbcrypto::KeyPair<lbcrypto::DCRTPoly>   keypair_;
};

// =============================================================================
// 1-3  Planner tests — structural correctness (no crypto execution)
// =============================================================================

TEST_F(JoinQueryTest, PlanJoinQuerySuccessfully)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("students");

    auto* p1 = stmt.add_projections();
    p1->mutable_column_ref()->set_table("students");
    p1->mutable_column_ref()->set_column("grade");

    auto* p2 = stmt.add_projections();
    p2->mutable_column_ref()->set_table("enrollments");
    p2->mutable_column_ref()->set_column("score");

    auto* join = stmt.add_joins();
    join->set_type(JOIN_TYPE_INNER);
    join->mutable_table()->set_table("enrollments");

    auto* cond = join->mutable_condition()->mutable_comparison_expr();
    cond->set_operator_(COMPARISON_OP_EQ);
    cond->mutable_left()->mutable_column_ref()->set_table("students");
    cond->mutable_left()->mutable_column_ref()->set_column("id");
    cond->mutable_right()->mutable_column_ref()->set_table("enrollments");
    cond->mutable_right()->mutable_column_ref()->set_column("student_id");

    auto plan = planner.plan(stmt);
    EXPECT_TRUE(plan.isValid());

    auto* proj_op = dynamic_cast<ProjectionOperator*>(&plan.getRoot());
    ASSERT_NE(proj_op, nullptr);
    EXPECT_NE(proj_op->getJoinChild(), nullptr);
}

TEST_F(JoinQueryTest, PlanJoinWithWhereClause)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("students");
    stmt.add_projections()->mutable_wildcard();

    auto* join = stmt.add_joins();
    join->set_type(JOIN_TYPE_INNER);
    join->mutable_table()->set_table("enrollments");

    auto* cond = join->mutable_condition()->mutable_comparison_expr();
    cond->set_operator_(COMPARISON_OP_EQ);
    cond->mutable_left()->mutable_column_ref()->set_column("id");
    cond->mutable_right()->mutable_column_ref()->set_column("student_id");

    auto* where = stmt.mutable_where()->mutable_comparison_expr();
    where->set_operator_(COMPARISON_OP_GT);
    where->mutable_left()->mutable_column_ref()->set_column("grade");
    where->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);

    auto plan = planner.plan(stmt);
    EXPECT_TRUE(plan.isValid());

    auto* proj_op = dynamic_cast<ProjectionOperator*>(&plan.getRoot());
    ASSERT_NE(proj_op, nullptr);

    auto* filter_op = proj_op->getFilterChild();
    ASSERT_NE(filter_op, nullptr);
    EXPECT_NE(dynamic_cast<JoinOperator*>(filter_op->getChild()), nullptr);
}

TEST_F(JoinQueryTest, PlanJoinThrowsOnNonExistentTable)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("students");
    stmt.add_projections()->mutable_wildcard();

    auto* join = stmt.add_joins();
    join->set_type(JOIN_TYPE_INNER);
    join->mutable_table()->set_table("ghost_table");

    EXPECT_THROW(planner.plan(stmt), std::runtime_error);
}

// =============================================================================
// 4-8  JoinOperator — mask correctness (no WHERE)
//
//  Data: students(N=2) id=[10,20] grade=[90,70]
//        enrollments(M=2) student_id=[10,20] score=[50,80]
//
//  Slot layout  j*N+i:
//    j=0, i=0 -> id=10 vs sid=10 -> MATCH  slot 0
//    j=0, i=1 -> id=20 vs sid=10 -> no     slot 1
//    j=1, i=0 -> id=10 vs sid=20 -> no     slot 2
//    j=1, i=1 -> id=20 vs sid=20 -> MATCH  slot 3
//
//  Expected join mask: [1, 0, 0, 1]
// =============================================================================

TEST_F(JoinQueryTest, JoinMaskTwoByTwo_PartialMatch)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        nullptr);

    ASSERT_GE(mask.size(), 4u);
    EXPECT_EQ(mask[0], 1);   // id=10 == student_id=10
    EXPECT_EQ(mask[1], 0);
    EXPECT_EQ(mask[2], 0);
    EXPECT_EQ(mask[3], 1);   // id=20 == student_id=20
}

TEST_F(JoinQueryTest, JoinMaskAllZeroWhenNoRowsMatch)
{
    // No student id appears in enrollment.student_id
    auto mask = runJoinQuery(
        makeStudentBatch(    {1, 2}, {90, 70}),
        makeEnrollmentBatch( {3, 4}, {50, 80}),
        nullptr);

    ASSERT_GE(mask.size(), 4u);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(mask[i], 0) << "Expected no match at slot " << i;
    }
}

TEST_F(JoinQueryTest, JoinMaskOneByOne_MatchingRows)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {42}, {88}),
        makeEnrollmentBatch( {42}, {75}),
        nullptr);

    ASSERT_GE(mask.size(), 1u);
    EXPECT_EQ(mask[0], 1);
}

TEST_F(JoinQueryTest, JoinBatchHasQualifiedAndUnqualifiedColumns)
{
    // After join, columns must be accessible via both "table.col" and "col".
    auto left_scan  = std::make_unique<MockScanOperator>(
        makeStudentBatch({10, 20}, {90, 70}));
    auto right_scan = std::make_unique<MockScanOperator>(
        makeEnrollmentBatch({10, 20}, {50, 80}));

    JoinOperator join_op(
        std::move(left_scan), std::move(right_scan),
        "students", "enrollments",
        makeJoinCondition());

    ExecutionContext ctx(catalog_, storage_, crypto_context_, RESULT_MODE_ROWS_AND_MASK);
    join_op.open(ctx);
    auto batch_opt = join_op.next();
    ASSERT_TRUE(batch_opt.has_value());
    auto& b = *batch_opt;

    // Qualified
    EXPECT_TRUE(b.hasColumn("students.id"));
    EXPECT_TRUE(b.hasColumn("students.grade"));
    EXPECT_TRUE(b.hasColumn("enrollments.student_id"));
    EXPECT_TRUE(b.hasColumn("enrollments.score"));
    // Unqualified
    EXPECT_TRUE(b.hasColumn("id"));
    EXPECT_TRUE(b.hasColumn("grade"));
    EXPECT_TRUE(b.hasColumn("student_id"));
    EXPECT_TRUE(b.hasColumn("score"));

    join_op.close();
}

TEST_F(JoinQueryTest, JoinRowCountIsNTimesM)
{
    // 3 left rows x 4 right rows = 12 joined rows
    auto left_scan  = std::make_unique<MockScanOperator>(
        makeStudentBatch({1, 2, 3}, {90, 80, 70}));
    auto right_scan = std::make_unique<MockScanOperator>(
        makeEnrollmentBatch({1, 2, 3, 4}, {60, 70, 80, 90}));

    JoinOperator join_op(
        std::move(left_scan), std::move(right_scan),
        "students", "enrollments",
        makeJoinCondition());

    ExecutionContext ctx(catalog_, storage_, crypto_context_, RESULT_MODE_ROWS_AND_MASK);
    join_op.open(ctx);
    auto batch_opt = join_op.next();
    ASSERT_TRUE(batch_opt.has_value());
    EXPECT_EQ(batch_opt->getRowCount(), 12u);
    join_op.close();
}

// =============================================================================
// 9-13  ExecutionEngine — join + WHERE combined mask
//
//  Data: students(N=2) id=[10,20] grade=[90,70]
//        enrollments(M=2) student_id=[10,20] score=[50,80]
//
//  Replicated grade in joined batch   slots[j*2+i]: [90, 70, 90, 70]
//  Replicated score in joined batch   slots[j*2+i]: [50, 50, 80, 80]
//  Join mask                                       : [ 1,  0,  0,  1]
//
//  Matched pairs:
//    slot 0: student id=10, grade=90, score=50
//    slot 3: student id=20, grade=70, score=80
// =============================================================================

// 9. WHERE grade > 80
//    Filter mask: grade>80 -> [1,0,1,0]
//    Combined   : AND with join mask [1,0,0,1] -> [1,0,0,0]
//    Survivor   : slot 0 (grade=90>80, id=10 matched sid=10)
TEST_F(JoinQueryTest, JoinWhereOnLeftColumn)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        makeWhere("grade", operations::ComparisonOp::GT, 0),
        { {0, 80} });

    ASSERT_GE(mask.size(), 4u);
    EXPECT_EQ(mask[0], 1);   // grade=90 > 80 AND join match -> survives
    EXPECT_EQ(mask[1], 0);
    EXPECT_EQ(mask[2], 0);
    EXPECT_EQ(mask[3], 0);   // grade=70 > 80 fails -> killed by filter
}

// 10. WHERE score > 60
//     Filter mask: score>60 -> [0,0,1,1]
//     Combined   : AND with join mask [1,0,0,1] -> [0,0,0,1]
//     Survivor   : slot 3 (score=80>60, id=20 matched sid=20)
TEST_F(JoinQueryTest, JoinWhereOnRightColumn)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        makeWhere("score", operations::ComparisonOp::GT, 0),
        { {0, 60} });

    ASSERT_GE(mask.size(), 4u);
    EXPECT_EQ(mask[0], 0);   // score=50 > 60 fails -> killed by filter
    EXPECT_EQ(mask[1], 0);
    EXPECT_EQ(mask[2], 0);
    EXPECT_EQ(mask[3], 1);   // score=80 > 60 AND join match -> survives
}

// 11. WHERE grade > 100 — impossible threshold, no grade exceeds 100
//     Filter mask: all 0
//     Combined   : all 0 (filter kills even the join matches)
TEST_F(JoinQueryTest, JoinWhereEliminatesAllJoinMatches)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        makeWhere("grade", operations::ComparisonOp::GT, 0),
        { {0, 100} });

    ASSERT_GE(mask.size(), 4u);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(mask[i], 0)
            << "Expected 0 at slot " << i << " (WHERE grade>100 is never true)";
    }
}

// 12. WHERE grade > 50 — all grades (90, 70) exceed 50
//     Filter mask: all 1
//     Combined   : same as join mask [1,0,0,1]
TEST_F(JoinQueryTest, JoinWherePreservesAllJoinMatches)
{
    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        makeWhere("grade", operations::ComparisonOp::GT, 0),
        { {0, 50} });

    ASSERT_GE(mask.size(), 4u);
    EXPECT_EQ(mask[0], 1);   // join match, grade>50
    EXPECT_EQ(mask[1], 0);   // no join match
    EXPECT_EQ(mask[2], 0);   // no join match
    EXPECT_EQ(mask[3], 1);   // join match, grade>50
}

// 13. WHERE grade > 80 AND score > 60
//     grade>80 filter mask : [1, 0, 1, 0]
//     score>60 filter mask : [0, 0, 1, 1]
//     AND filter mask      : [0, 0, 1, 0]
//     join mask            : [1, 0, 0, 1]
//     combined (AND)       : [0, 0, 0, 0]
//     No pair satisfies both WHERE conditions AND the join key.
TEST_F(JoinQueryTest, JoinAndWhereCompoundPredicate)
{
    ExprPtr grade_gt_80 = makeWhere("grade", operations::ComparisonOp::GT, 0);
    ExprPtr score_gt_60 = makeWhere("score", operations::ComparisonOp::GT, 1);
    ExprPtr compound    = makeAndWhere(grade_gt_80, score_gt_60);

    auto mask = runJoinQuery(
        makeStudentBatch(    {10, 20}, {90, 70}),
        makeEnrollmentBatch( {10, 20}, {50, 80}),
        compound,
        { {0, 80}, {1, 60} });

    ASSERT_GE(mask.size(), 4u);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(mask[i], 0)
            << "Expected 0 at slot " << i
            << " (no pair satisfies grade>80 AND score>60 AND join key)";
    }
}
