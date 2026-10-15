/**
 * test_query_types.cpp
 *
 * Tests different query types against the persistent EMPLOYEES database.
 * All execution goes through the server/ components:
 *   QueryExecutor → QueryPlanner → ExecutionEngine
 *       → TableScanOperator / FilterOperator / ProjectionOperator
 *       → ComparisonEvaluator / LogicalEvaluator / PredicateEvaluator
 *
 * Queries tested:
 *   1. SELECT * FROM EMPLOYEES                       (baseline)
 *   2. SELECT id, salary FROM EMPLOYEES              (column projection)
 *   3. SELECT * FROM EMPLOYEES WHERE id = ?          (eq filter, integer)
 *   4. SELECT * FROM EMPLOYEES WHERE salary > ?      (gt filter)
 *   5. SELECT * FROM EMPLOYEES WHERE age >= ? AND age <= ?   (range, AND)
 *   6. SELECT id, dept FROM EMPLOYEES WHERE dept = ? (string column projection + filter)
 *
 * Expected data (from hegadb_setup):
 *   id  age  salary  dept
 *   1   25   75   Engineering
 *   2   32   62   Sales
 *   3   30   85   Engineering
 *   4   45   95   Management
 *   5   28   80   Engineering
 *   6   35   68   Sales
 *   7   29   72   Marketing
 *   8   42   90   Management
 *   9   26   65   Marketing
 *  10   38   78   Sales
 */

// Client components
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/Wildcard.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>

// Server components
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>

// Catalog
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>

// Crypto
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>

// Client-side decryption / display
#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/execution/ResultFormatter.h>

// Types
#include <hegadb/types/DataType.h>

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <functional>

using namespace hegadb;

// ============================================================
// Test harness
// ============================================================

struct TestContext {
    std::shared_ptr<crypto::CompositeEncryption>   encryption;
    std::shared_ptr<crypto::OpenFHEContext>         crypto_context;
    std::shared_ptr<catalog::InMemoryCatalog>       catalog;
    std::shared_ptr<server::storage::StorageManager> storage;
};

// Column metadata for the EMPLOYEES table (matches hegadb_setup output)
static std::vector<catalog::ColumnSchema> const EMPLOYEES_COLS = {
    {"id",     types::DataType::Integer, false, 0},
    {"age",    types::DataType::Integer, false, 1},
    {"salary", types::DataType::Integer, false, 2},
    {"dept",   types::DataType::String,  false, 3},
};

TestContext setup() {
    TestContext ctx;
    std::string home        = std::getenv("HOME");
    std::string data_dir    = home + "/.hegadb/data";
    std::string keys_dir    = home + "/.hegadb/keys";
    std::string catalog_dir = home + "/.hegadb/catalog";

    // Load encryption (keys on disk)
    ctx.encryption = std::make_shared<crypto::CompositeEncryption>(keys_dir);

    // Wire crypto context from loaded keys
    ctx.crypto_context = std::make_shared<crypto::OpenFHEContext>();
    ctx.crypto_context->setBFVContext(ctx.encryption->getContext().getBFVContext());
    ctx.crypto_context->setCKKSContext(ctx.encryption->getContext().getCKKSContext());

    // Build catalog
    std::vector<catalog::ColumnSchema> cols = EMPLOYEES_COLS;
    catalog::TableSchema schema("EMPLOYEES", std::move(cols));
    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("EMPLOYEES", std::move(schema));
    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables));
    ctx.catalog = std::make_shared<catalog::InMemoryCatalog>(snapshot);

    // Storage
    ctx.storage = server::storage::StorageManager::createFileStorage(data_dir, catalog_dir);

    return ctx;
}

// Run one query through the full server pipeline.
// Returns DecryptedResult — columns/types derived from the SELECT list.
execution::DecryptedResult run_query(
    TestContext const& ctx,
    std::string const& sql,
    std::string const& test_name)
{
    std::cout << "\n" << std::string(60, '─') << "\n";
    std::cout << "Test: " << test_name << "\n";
    std::cout << "SQL:  " << sql << "\n";
    std::cout << std::string(60, '─') << "\n";

    // ── Client: parse ──────────────────────────────────────────
    client::SQLParser parser;
    auto ast = parser.parseToAST(sql);
    if (!ast) throw std::runtime_error("Parse failed: " + sql);

    auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
    if (!select_stmt) throw std::runtime_error("Not a SELECT");

    // ── Client: validate ───────────────────────────────────────
    client::semantic::SemanticValidator validator(*ctx.catalog);
    validator.validate(*select_stmt);

    // ── Determine projected columns (for decryption) ───────────
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

    // ── Client: prepare (encrypt literal parameters) ──────────
    client::preparation::QueryPreparer preparer(*ctx.encryption);
    auto prepared = preparer.prepare(std::move(ast));
    std::cout << "  Parameters: " << prepared.parameters().size() << " encrypted\n";

    // ── Client: encode protocol message ───────────────────────
    auto request = client::protocol::ExecuteQueryEncoder::encode(
        prepared, "test-" + test_name);

    // ── SERVER: execute ────────────────────────────────────────
    server::QueryExecutor executor(ctx.catalog, ctx.storage, ctx.crypto_context);
    auto result = executor.execute(request);

    std::cout << "  Server: " << result.getBatches().size() << " batch(es), "
              << result.getTotalRowCount() << " row(s) total\n";
    std::cout << "  Mask present: " << (result.hasMask() ? "yes" : "no") << "\n";

    // ── Client: decrypt ────────────────────────────────────────
    execution::ResultDecryptor decryptor(ctx.encryption);
    auto decrypted = decryptor.decrypt(result, col_names, col_types);

    // ── Display ────────────────────────────────────────────────
    execution::FormatConfig fmt;
    fmt.format = execution::DisplayFormat::Table;
    execution::ResultFormatter formatter(fmt);
    formatter.format(decrypted);

    // Print which rows the mask says matched
    size_t matched = 0;
    for (auto const& row : decrypted.rows) {
        if (row.matches_filter) ++matched;
    }
    std::cout << "  Filter matched: " << matched << " / " << decrypted.rows.size() << " rows\n";

    return decrypted;
}

// ============================================================
// main
// ============================================================

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║   HEGA-DB Query Types Integration Test                  ║\n";
    std::cout << "║   File-backed EMPLOYEES table, server/ execution path   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";

    TestContext ctx;
    try {
        std::cout << "\nInitialising...\n";
        ctx = setup();
        std::cout << "  ✓ Keys loaded, catalog built, storage ready\n";
    } catch (std::exception const& e) {
        std::cerr << "Setup failed: " << e.what() << "\n";
        std::cerr << "Have you run:  ./tools/setup/hegadb_setup --init-database ?\n";
        return 1;
    }

    int failed = 0;
    auto run = [&](std::string sql, std::string name, size_t expected_matches) {
        try {
            auto result = run_query(ctx, sql, name);
            size_t actual_matches = 0;
            for (auto const& row : result.rows) {
                if (row.matches_filter) ++actual_matches;
            }
            if (actual_matches != expected_matches) {
                throw std::runtime_error(
                    "Expected " + std::to_string(expected_matches) +
                    " matching row(s), got " + std::to_string(actual_matches));
            }
        } catch (std::exception const& e) {
            std::cerr << "  ✗ EXCEPTION: " << e.what() << "\n";
            ++failed;
        }
    };

    // ── Test 1: SELECT * ───────────────────────────────────────────
    run("SELECT * FROM EMPLOYEES",
        "1_select_star",
        10);

    // ── Test 2: Column projection ──────────────────────────────────
    run("SELECT id, salary FROM EMPLOYEES",
        "2_column_projection",
        10);

    // ── Test 3: WHERE with integer equality ────────────────────────
    // Tests: parameter encryption, FilterOperator, ComparisonEvaluator (EQ),
    //        mask production and per-slot decryption.
    // Enc(id - param) → slot[0] = 1-1 = 0 (match), others non-zero (no match).
    run("SELECT * FROM EMPLOYEES WHERE id = 3",
        "3_where_eq_integer",
        1);

    // ── Test 4: WHERE with GT ──────────────────────────────────────
    // Enc(salary - param) in each slot. After client decryption,
    // positive diff → salary > threshold.
    run("SELECT id, salary FROM EMPLOYEES WHERE salary > 80",
        "4_where_gt",
        3);

    // ── Test 5: WHERE with AND (range query) ──────────────────────
    // age >= 28 AND age <= 35 → ids 2,3,5,6,7 (5 matches)
    run("SELECT id, age FROM EMPLOYEES WHERE age >= 28 AND age <= 35",
        "5_where_and_range",
        5);

    // ── Test 6: WHERE with OR ──────────────────────────────────────
    run("SELECT id, dept FROM EMPLOYEES WHERE id = 1 OR id = 5",
        "6_where_or",
        2);

    // ── Test 7: WHERE with NOT ─────────────────────────────────────
    run("SELECT id, age FROM EMPLOYEES WHERE NOT age = 25",
        "7_where_not",
        9);

    // ── Test 8: Column projection + WHERE ─────────────────────────
    run("SELECT id, dept FROM EMPLOYEES WHERE salary = 95",
        "8_projection_with_filter",
        1);

    // ── Summary ───────────────────────────────────────────────────
    std::cout << "\n" << std::string(60, '═') << "\n";
    if (failed == 0) {
        std::cout << "All tests ran without exceptions.\n";
        std::cout << "Review the tables above to verify correctness of\n";
        std::cout << "column projection and filter mask values.\n";
    } else {
        std::cout << failed << " test(s) threw exceptions — see output above.\n";
    }
    std::cout << std::string(60, '═') << "\n\n";

    return failed > 0 ? 1 : 0;
}
