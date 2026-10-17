#include <hegadb/server/operators/JoinOperator.h>
#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <hegadb/logging/Logger.h>
#include <stdexcept>
#include <vector>

namespace hegadb::server::operators {

JoinOperator::JoinOperator(
    std::unique_ptr<Operator> left_child,
    std::unique_ptr<Operator> right_child,
    std::string left_table_name,
    std::string right_table_name,
    planner::ExprPtr join_condition)
    : left_child_(std::move(left_child))
    , right_child_(std::move(right_child))
    , left_table_name_(std::move(left_table_name))
    , right_table_name_(std::move(right_table_name))
    , join_condition_(std::move(join_condition))
{
}

void JoinOperator::open(execution::ExecutionContext& context)
{
    context_ = &context;
    left_child_->open(context);
    right_child_->open(context);
    executed_ = false;
    accumulated_mask_.reset();
}

crypto::OpenFHECiphertext JoinOperator::replicateLeftColumn(
    crypto::OpenFHECiphertext const& col,
    size_t n,
    size_t m,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // Replicate left table (N rows) across M blocks:
    // Slot (j * n + i) = Left[i] for j in [0, m-1]
    auto ring_dim = ctx->GetRingDimension();
    std::vector<int64_t> mask_vec(ring_dim, 0);
    for (size_t i = 0; i < n; ++i) {
        mask_vec[i] = 1;
    }
    auto pt_mask = ctx->MakePackedPlaintext(mask_vec);
    auto base_ct = ctx->EvalMult(col.getCiphertext(), pt_mask);

    auto result = base_ct;
    for (size_t j = 1; j < m; ++j) {
        auto shifted = ctx->EvalRotate(base_ct, -static_cast<int32_t>(j * n));
        result = ctx->EvalAdd(result, shifted);
    }

    return crypto::OpenFHECiphertext(result, col.getSchemeType(), col.getOriginalType());
}

crypto::OpenFHECiphertext JoinOperator::replicateRightColumn(
    crypto::OpenFHECiphertext const& col,
    size_t n,
    size_t m,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // Replicate right table (M rows):
    // Slot (j * n + i) = Right[j] for i in [0, n-1]
    auto ring_dim = ctx->GetRingDimension();
    auto raw_ct = col.getCiphertext();
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> result = nullptr;

    for (size_t j = 0; j < m; ++j) {
        std::vector<int64_t> single_slot_mask(ring_dim, 0);
        single_slot_mask[j] = 1;
        auto pt_single = ctx->MakePackedPlaintext(single_slot_mask);
        auto isolated = ctx->EvalMult(raw_ct, pt_single);

        // Rotate slot j to slot 0
        if (j > 0) {
            isolated = ctx->EvalRotate(isolated, static_cast<int32_t>(j));
        }

        // Broadcast slot 0 to slots 0..n-1
        std::vector<int64_t> block_ones(ring_dim, 0);
        for (size_t i = 0; i < n; ++i) {
            block_ones[i] = 1;
        }
        auto pt_block = ctx->MakePackedPlaintext(block_ones);

        // Duplicate across n slots by powers-of-two rotations
        for (size_t step = 1; step < n; step *= 2) {
            auto rotated = ctx->EvalRotate(isolated, -static_cast<int32_t>(step));
            isolated = ctx->EvalAdd(isolated, rotated);
        }
        isolated = ctx->EvalMult(isolated, pt_block);

        // Place at block j
        if (j > 0) {
            isolated = ctx->EvalRotate(isolated, -static_cast<int32_t>(j * n));
        }

        if (!result) {
            result = isolated;
        } else {
            result = ctx->EvalAdd(result, isolated);
        }
    }

    return crypto::OpenFHECiphertext(result, col.getSchemeType(), col.getOriginalType());
}

std::optional<execution::ExecutionBatch> JoinOperator::next()
{
    if (executed_ || !context_) {
        return std::nullopt;
    }
    executed_ = true;

    auto left_batch = left_child_->next();
    auto right_batch = right_child_->next();

    if (!left_batch || !right_batch) {
        return std::nullopt;
    }

    size_t n = left_batch->getRowCount();
    size_t m = right_batch->getRowCount();
    size_t total_joined_rows = n * m;

    if (total_joined_rows == 0) {
        return std::nullopt;
    }

    logging::debug("JoinOperator: Joining {} rows (left: {}) with {} rows (right: {}) -> total pairs: {}",
        left_table_name_, n, right_table_name_, m, total_joined_rows);

    auto ctx = context_->getCryptoContext().getBFVContext();
    execution::ExecutionBatch joined_batch;
    joined_batch.setRowCount(total_joined_rows);

    // 1. Replicate Left Table Columns
    for (auto const& col_name : left_batch->getColumnNames()) {
        auto rep_ct = replicateLeftColumn(left_batch->getColumn(col_name), n, m, ctx);
        // Expose as both fully qualified "Table.col" and simple "col"
        joined_batch.addColumn(left_table_name_ + "." + col_name, rep_ct);
        if (!joined_batch.hasColumn(col_name)) {
            joined_batch.addColumn(col_name, rep_ct);
        }
    }

    // 2. Replicate Right Table Columns
    for (auto const& col_name : right_batch->getColumnNames()) {
        auto rep_ct = replicateRightColumn(right_batch->getColumn(col_name), n, m, ctx);
        joined_batch.addColumn(right_table_name_ + "." + col_name, rep_ct);
        if (!joined_batch.hasColumn(col_name)) {
            joined_batch.addColumn(col_name, rep_ct);
        }
    }

    // 3. Evaluate Equi-Join Condition
    auto const* comp = std::get_if<planner::ComparisonExpr>(&(*join_condition_));
    if (!comp || comp->op != operations::ComparisonOp::EQ) {
        throw std::runtime_error("JoinOperator only supports equi-join (EQ) condition");
    }

    auto get_col_ciphertext = [&](planner::ExprPtr const& expr) -> crypto::OpenFHECiphertext {
        auto const* col_ref = std::get_if<planner::ColumnRefExpr>(&(*expr));
        if (!col_ref) {
            throw std::runtime_error("Join condition operand must be a ColumnRef");
        }
        std::string qualified = (!col_ref->table.empty()) ? (col_ref->table + "." + col_ref->column) : col_ref->column;
        if (joined_batch.hasColumn(qualified)) {
            return joined_batch.getColumn(qualified);
        }
        if (joined_batch.hasColumn(col_ref->column)) {
            return joined_batch.getColumn(col_ref->column);
        }
        throw std::runtime_error("Column not found in joined batch: " + qualified);
    };

    auto left_key_ct = get_col_ciphertext(comp->left);
    auto right_key_ct = get_col_ciphertext(comp->right);

    evaluator::ComparisonEvaluator comp_eval(context_->getCryptoContext());
    auto join_mask = comp_eval.evaluate(left_key_ct, operations::ComparisonOp::EQ, right_key_ct);

    accumulated_mask_ = std::make_unique<execution::EncryptedMask>(std::move(join_mask));

    return joined_batch;
}

void JoinOperator::close()
{
    if (left_child_) left_child_->close();
    if (right_child_) right_child_->close();
}

execution::EncryptedMask JoinOperator::getAccumulatedMask() const
{
    if (!accumulated_mask_) {
        throw std::runtime_error("No accumulated join mask available");
    }
    return *accumulated_mask_;
}

} // namespace hegadb::server::operators
