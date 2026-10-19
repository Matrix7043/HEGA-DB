/**
 * test_grpc_transport.cpp
 *
 * End-to-end transport integration test.
 *
 * Exercises the COMPLETE stack that the production server uses:
 *
 *   Client side:
 *     SQLParser → SemanticValidator → QueryPreparer → ExecuteQueryEncoder
 *     → GrpcClient::execute()
 *          ↕  (real TCP/gRPC wire)
 *   Server side:
 *     GrpcServer → HEGADBServiceImpl → ExecuteQueryReceiver
 *     → QueryExecutor → ExecutionEngine
 *     → ResultEncoder (ciphertexts serialised into QueryResponse)
 *
 *   Client side (after wire):
 *     ResultDecryptor → DecryptedResult
 *
 * The test uses the persistent file-backed EMPLOYEES table written by
 * hegadb_setup.  Run that tool first:
 *
 *   ./tools/setup/hegadb_setup --init-database
 *
 * The test exits 0 with a skip message when the EMPLOYEES data directory
 * is not present, so CI passes even without pre-loaded data.
 *
 * Queries tested:
 *   T1  SELECT * FROM EMPLOYEES                                (baseline)
 *   T2  SELECT id, salary FROM EMPLOYEES                       (projection)
 *   T3  SELECT * FROM EMPLOYEES WHERE id = 3                   (eq filter)
 *   T4  SELECT id, salary FROM EMPLOYEES WHERE salary > 80     (gt filter)
 *   T5  SELECT id, age FROM EMPLOYEES WHERE age >= 28 AND age <= 35  (range/AND)
 *   T6  SELECT id, dept FROM EMPLOYEES WHERE id = 1 OR id = 5  (OR)
 *   T7  SELECT id, age FROM EMPLOYEES WHERE NOT age = 25        (NOT)
 */

// ── Transport ──────────────────────────────────────────────────────────────
#include <hegadb/transport/grpc/GrpcClient.h>
#include <hegadb/transport/grpc/GrpcServer.h>

// ── Server components ──────────────────────────────────────────────────────
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/storage/StorageManager.h>

// ── Client components ──────────────────────────────────────────────────────
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/Wildcard.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>

// ── Catalog ────────────────────────────────────────────────────────────────
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>

// ── Crypto ─────────────────────────────────────────────────────────────────
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>

// ── Execution utilities (client-side decryption) ───────────────────────────
#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/execution/ResultFormatter.h>

// ── Types ──────────────────────────────────────────────────────────────────
#include <hegadb/types/DataType.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace hegadb;

// ============================================================
// EMPLOYEES table schema (must match hegadb_setup output)
// ============================================================

static std::vector<catalog::ColumnSchema> const EMPLOYEES_COLS = {
    {"id",     types::DataType::Integer, false, 0},
    {"age",    types::DataType::Integer, false, 1},
    {"salary", types::DataType::Integer, false, 2},
    {"dept",   types::DataType::String,  false, 3},
};

// ============================================================
// Test context
// ============================================================

struct GrpcTransportTestContext {
    std::shared_ptr<crypto::CompositeEncryption>    encryption;
    std::shared_ptr<crypto::OpenFHEContext>          crypto_context;
    std::shared_ptr<catalog::InMemoryCatalog>        catalog;
    std::shared_ptr<server::storage::StorageManager> storage;
    std::unique_ptr<transport::grpc::GrpcServer>     server;
    std::string                                       server_address;
};

// ============================================================
// Catalog builder
// ============================================================

static std::shared_ptr<catalog::InMemoryCatalog> buildCatalog()
{
    std::vector<catalog::ColumnSchema> cols = EMPLOYEES_COLS;
    catalog::TableSchema schema("EMPLOYEES", std::move(cols));
    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("EMPLOYEES", std::move(schema));
    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(
        1, std::move(tables));
    return std::make_shared<catalog::InMemoryCatalog>(snapshot);
}

// ============================================================
// Server setup — returns nullopt if EMPLOYEES data is absent
// ============================================================

static std::optional<GrpcTransportTestContext> startTestServer()
{
    std::string home        = std::getenv("HOME");
    std::string data_dir    = home + "/.hegadb/data";
    std::string keys_dir    = home + "/.hegadb/keys";
    std::string catalog_dir = home + "/.hegadb/catalog";

    if (!std::filesystem::exists(keys_dir)) {
        std::cout << "[SKIP] Keys not found at: " << keys_dir << "\n"
                  << "[SKIP] Run: ./tools/setup/hegadb_setup --init-database\n";
        return std::nullopt;
    }

    GrpcTransportTestContext ctx;

    // Crypto
    ctx.encryption     = std::make_shared<crypto::CompositeEncryption>(keys_dir);
    ctx.crypto_context = std::make_shared<crypto::OpenFHEContext>();
    ctx.crypto_context->setBFVContext(ctx.encryption->getContext().getBFVContext());
    ctx.crypto_context->setCKKSContext(ctx.encryption->getContext().getCKKSContext());

    // Catalog + file-backed storage
    ctx.catalog = buildCatalog();
    ctx.storage = server::storage::StorageManager::createFileStorage(data_dir, catalog_dir);

    // QueryExecutor + receiver
    auto executor = std::make_shared<server::QueryExecutor>(
        ctx.catalog, ctx.storage, ctx.crypto_context);
    auto receiver = std::make_shared<server::ExecuteQueryReceiver>(
        ctx.catalog, executor);

    // gRPC server on OS-assigned port (0 = let the OS pick)
    ctx.server = std::make_unique<transport::grpc::GrpcServer>("127.0.0.1:0");
    ctx.server->setStreamingRequestHandler(
        [receiver](protocol::ExecuteQuery const& request,
                   transport::ServerTransport::BatchWriter writer) {
            receiver->executeQueryStream(request, std::move(writer));
        });
    ctx.server->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    ctx.server_address = ctx.server->address();

    std::cout << "[SETUP] gRPC server listening on " << ctx.server_address << "\n";
    return ctx;
}

// ============================================================
// Per-query runner: SQL → GrpcClient → ResultDecryptor
// ============================================================

struct QueryRunResult {
    execution::DecryptedResult decrypted;
    size_t                     matched = 0;
};

static QueryRunResult runOverGrpc(
    GrpcTransportTestContext const& ctx,
    std::string const&              sql,
    std::string const&              test_name)
{
    std::cout << "\n" << std::string(64, '-') << "\n";
    std::cout << "Test : " << test_name << "\n";
    std::cout << "SQL  : " << sql << "\n";
    std::cout << std::string(64, '-') << "\n";

    // ── Parse ────────────────────────────────────────────────
    client::SQLParser parser;
    auto ast = parser.parseToAST(sql);
    if (!ast) throw std::runtime_error("Parse failed: " + sql);

    auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
    if (!select_stmt) throw std::runtime_error("Not a SELECT statement");

    // ── Validate ─────────────────────────────────────────────
    client::semantic::SemanticValidator validator(*ctx.catalog);
    validator.validate(*select_stmt);

    // ── Determine projected columns (for client-side decryption) ──
    std::vector<std::string>     col_names;
    std::vector<types::DataType> col_types;
    auto* tschema = ctx.catalog->findTable(select_stmt->from()->name());

    bool wildcard = false;
    for (auto const& p : select_stmt->projections())
        if (p->kind() == client::ast::ASTNodeKind::Wildcard) { wildcard = true; break; }

    if (wildcard) {
        for (auto const& c : tschema->columns()) {
            col_names.push_back(c.name);
            col_types.push_back(c.type);
        }
    } else {
        for (auto const& p : select_stmt->projections()) {
            if (p->kind() == client::ast::ASTNodeKind::ColumnRef) {
                auto* cr = dynamic_cast<client::ast::ColumnRef const*>(p.get());
                auto* cs = tschema->findColumn(cr->name());
                if (cs) { col_names.push_back(cs->name); col_types.push_back(cs->type); }
            }
        }
    }

    // ── Determine mask kind from query AST (before ast is moved) ──
    server::execution::MaskKind mask_kind = server::execution::MaskKind::COMPOSED;
    if (select_stmt->where()) {
        if (auto const* comp = dynamic_cast<client::ast::ComparisonExpr const*>(select_stmt->where())) {
            switch (comp->op()) {
                // ComparisonEvaluator returns 0/1 indicator masks for every
                // comparison operator, so matching slots are non-zero.
                case operations::ComparisonOp::EQ:  mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
                case operations::ComparisonOp::NEQ: mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
                case operations::ComparisonOp::LT:  mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
                case operations::ComparisonOp::GT:  mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
                case operations::ComparisonOp::LTE: mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
                case operations::ComparisonOp::GTE: mask_kind = server::execution::MaskKind::DIFF_NONZERO; break;
            }
        }
    }

    // ── Prepare (encrypt literals) ───────────────────────────
    client::preparation::QueryPreparer preparer(*ctx.encryption);
    auto prepared = preparer.prepare(std::move(ast));
    std::cout << "  [client] " << prepared.parameters().size()
              << " parameter(s) encrypted\n";

    // ── Encode Protobuf message ──────────────────────────────
    auto request = client::protocol::ExecuteQueryEncoder::encode(
        prepared, "grpc-e2e-" + test_name);

    // ── Send over real gRPC ──────────────────────────────────
    transport::grpc::GrpcClient grpc_client(ctx.server_address);
    std::cout << "  [client] Sending query over gRPC...\n";
    protocol::QueryResponse response = grpc_client.execute(request);

    // ── Check transport-level status ─────────────────────────
    if (!response.has_status() ||
        response.status().code() != protocol::STATUS_OK)
    {
        throw std::runtime_error(
            "Server returned error: " + response.status().message());
    }

    auto const& rp = response.result();
    std::cout << "  [wire->client] row_count=" << rp.row_count()
              << "  columns=" << rp.columns_size()
              << "  mask=" << (rp.encrypted_mask().empty() ? "no" : "yes")
              << "\n";

    // ── Re-materialise ExecutionResult from the wire proto ────
    // This mirrors what a real client library would do.
    server::execution::ExecutionResult exec_result;
    exec_result.setResultMode(rp.mode());


    // Deserialise encrypted mask (if present)
    if (!rp.encrypted_mask().empty()) {
        // The mask was serialised as a ciphertext; reconstruct it.
        auto mask_ct = crypto::OpenFHECiphertext::deserialize(rp.encrypted_mask());
        server::execution::EncryptedMask mask(
            std::move(mask_ct),
            rp.row_count(),
            mask_kind
        );
        exec_result.setEncryptedMask(std::move(mask));
    }

    // Deserialise column batches
    if (rp.columns_size() > 0) {
        server::execution::ExecutionBatch batch;
        for (auto const& col_proto : rp.columns()) {
            // Each column may carry multiple per-segment ciphertexts.
            // For a single-batch table there is exactly one.
            for (auto const& serialised_ct : col_proto.ciphertexts()) {
                auto ct = crypto::OpenFHECiphertext::deserialize(serialised_ct);
                batch.addColumn(col_proto.column_name(), std::move(ct));
            }
        }
        batch.setRowCount(rp.row_count());
        exec_result.addBatch(std::move(batch));
    }

    // ── Client: decrypt ──────────────────────────────────────
    execution::ResultDecryptor decryptor(ctx.encryption);
    auto decrypted = decryptor.decrypt(exec_result, col_names, col_types);

    // ── Display ──────────────────────────────────────────────
    execution::FormatConfig fmt;
    fmt.format = execution::DisplayFormat::Table;
    execution::ResultFormatter formatter(fmt);
    formatter.format(decrypted);

    size_t matched = 0;
    for (auto const& row : decrypted.rows)
        if (row.matches_filter) ++matched;

    std::cout << "  Filter matched: " << matched
              << " / " << decrypted.rows.size() << " rows\n";

    return QueryRunResult{std::move(decrypted), matched};
}

// ============================================================
// main
// ============================================================

int main()
{
    std::cout
        << "╔══════════════════════════════════════════════════════════════╗\n"
        << "║  HEGA-DB  gRPC Transport End-to-End Integration Test         ║\n"
        << "║  Full stack: GrpcClient -> wire -> GrpcServer -> Executor    ║\n"
        << "║             -> ResultEncoder -> wire -> ResultDecryptor       ║\n"
        << "╚══════════════════════════════════════════════════════════════╝\n\n";

    auto maybe_ctx = startTestServer();
    if (!maybe_ctx) return 0;   // graceful skip — data not present
    auto& ctx = *maybe_ctx;

    int failed = 0;

    auto run = [&](std::string sql, std::string name,
                   std::function<void(QueryRunResult const&)> verify = {})
    {
        try {
            auto r = runOverGrpc(ctx, sql, name);
            if (verify) verify(r);
            std::cout << "  PASS " << name << "\n";
        } catch (std::exception const& e) {
            std::cerr << "  FAIL " << name << ": " << e.what() << "\n";
            ++failed;
        }
    };

    // T1 – SELECT * (baseline, no filter)
    // Expect 10 rows, all matched (no WHERE → ALL_MATCH)
    run("SELECT * FROM EMPLOYEES",
        "T1_select_star",
        [](QueryRunResult const& r) {
            if (r.decrypted.rows.size() != 10)
                throw std::runtime_error(
                    "Expected 10 rows, got " +
                    std::to_string(r.decrypted.rows.size()));
            if (r.matched != 10)
                throw std::runtime_error(
                    "Expected all 10 rows matched (no WHERE), got " +
                    std::to_string(r.matched));
        });

    // T2 – column projection
    // Expect 10 rows, 2 columns each
    run("SELECT id, salary FROM EMPLOYEES",
        "T2_column_projection",
        [](QueryRunResult const& r) {
            if (r.decrypted.rows.size() != 10)
                throw std::runtime_error(
                    "Expected 10 rows, got " +
                    std::to_string(r.decrypted.rows.size()));
            if (!r.decrypted.rows.empty() &&
                r.decrypted.rows[0].values.size() != 2)
                throw std::runtime_error("Expected 2 projected columns per row");
        });

    // T3 – WHERE id = 3
    // Only row 3 matches → 1 match
    run("SELECT * FROM EMPLOYEES WHERE id = 3",
        "T3_where_eq_integer",
        [](QueryRunResult const& r) {
            if (r.matched != 1)
                throw std::runtime_error(
                    "WHERE id=3: expected 1 match, got " +
                    std::to_string(r.matched));
        });

    // T4 – WHERE salary > 80
    // Salaries: 75,62,85,95,80,68,72,90,65,78
    // > 80: 85(id=3), 95(id=4), 90(id=8) → 3 matches
    run("SELECT id, salary FROM EMPLOYEES WHERE salary > 80",
        "T4_where_gt",
        [](QueryRunResult const& r) {
            if (r.matched != 3)
                throw std::runtime_error(
                    "WHERE salary>80: expected 3 matches, got " +
                    std::to_string(r.matched));
        });

    // T5 – WHERE age >= 28 AND age <= 35 (range / AND)
    // The range includes ages 32, 30, 28, 35, and 29: 5 matches.
    run("SELECT id, age FROM EMPLOYEES WHERE age >= 28 AND age <= 35",
        "T5_where_and_range",
        [](QueryRunResult const& r) {
            if (r.decrypted.rows.size() != 10)
                throw std::runtime_error("Expected 10 rows");
            if (r.matched != 5)
                throw std::runtime_error(
                    "WHERE age>=28 AND age<=35: expected 5 matches, got " +
                    std::to_string(r.matched));
        });

    // T6 – WHERE id = 1 OR id = 5 (OR)
    // Rows with id 1 and id 5 match: 2 matches.
    run("SELECT id, dept FROM EMPLOYEES WHERE id = 1 OR id = 5",
        "T6_where_or",
        [](QueryRunResult const& r) {
            if (r.decrypted.rows.size() != 10)
                throw std::runtime_error("Expected 10 rows");
            if (r.matched != 2)
                throw std::runtime_error(
                    "WHERE id=1 OR id=5: expected 2 matches, got " +
                    std::to_string(r.matched));
        });

    // T7 – WHERE NOT age = 25 (NOT)
    // Only age 25 is excluded, so 9 rows match.
    run("SELECT id, age FROM EMPLOYEES WHERE NOT age = 25",
        "T7_where_not",
        [](QueryRunResult const& r) {
            if (r.decrypted.rows.size() != 10)
                throw std::runtime_error("Expected 10 rows");
            if (r.matched != 9)
                throw std::runtime_error(
                    "WHERE NOT age=25: expected 9 matches, got " +
                    std::to_string(r.matched));
        });

    // ── Teardown ─────────────────────────────────────────────
    ctx.server->shutdown();

    // ── Summary ──────────────────────────────────────────────
    std::cout << "\n" << std::string(64, '=') << "\n";
    if (failed == 0) {
        std::cout << "All gRPC transport end-to-end tests passed.\n";
    } else {
        std::cerr << failed << " test(s) failed -- see output above.\n";
    }
    std::cout << std::string(64, '=') << "\n\n";

    return (failed > 0) ? 1 : 0;
}
