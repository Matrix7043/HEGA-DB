#include "InMemoryStorageManager.h"

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
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/planner/QueryPlanner.h>

// Catalog and crypto
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/BFVEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>

// Execution utilities
#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/execution/ResultFormatter.h>
#include <hegadb/types/DataType.h>

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <cassert>
#include <algorithm>

using namespace hegadb;

struct StudentRecord {
    int64_t id;
    int64_t age;
    int64_t score;
};

static const std::vector<StudentRecord> DUMMY_DATA = {
    {1, 20, 85},
    {2, 22, 90},
    {3, 25, 70},
    {4, 28, 95},
    {5, 30, 60},
    {6, 22, 75},
    {7, 35, 88},
    {8, 20, 92}
};

struct TestHarness {
    std::shared_ptr<crypto::CompositeEncryption>    encryption;
    std::shared_ptr<crypto::OpenFHEContext>          crypto_context;
    std::shared_ptr<catalog::InMemoryCatalog>        catalog;
    std::shared_ptr<test::InMemoryStorageManager>    storage;
};

void printPlaintextTable(std::vector<StudentRecord> const& records) {
    std::cout << "┌──────┬─────┬───────┐\n";
    std::cout << "│  ID  │ AGE │ SCORE │\n";
    std::cout << "├──────┼─────┼───────┤\n";
    for (auto const& r : records) {
        std::cout << "│ " << std::setw(4) << r.id
                  << " │ " << std::setw(3) << r.age
                  << " │ " << std::setw(5) << r.score << " │\n";
    }
    std::cout << "└──────┴─────┴───────┘\n";
}

TestHarness setupDummyTable() {
    TestHarness harness;

    std::cout << "\n========================================================================\n";
    std::cout << "  PHASE 1: Cryptographic Context & In-Memory Key Initialization\n";
    std::cout << "========================================================================\n";

    auto t0 = std::chrono::high_resolution_clock::now();
    harness.encryption = std::make_shared<crypto::CompositeEncryption>();
    harness.crypto_context = std::make_shared<crypto::OpenFHEContext>();
    harness.crypto_context->setBFVContext(harness.encryption->getContext().getBFVContext());
    harness.crypto_context->setCKKSContext(harness.encryption->getContext().getCKKSContext());
    auto t1 = std::chrono::high_resolution_clock::now();
    double init_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "  ✓ OpenFHE Context & BFV Keys initialized in " << std::fixed << std::setprecision(2)
              << init_ms << " ms\n";

    std::cout << "\n========================================================================\n";
    std::cout << "  PHASE 2: Dummy Table Creation & Vector Packing Encryption\n";
    std::cout << "========================================================================\n";
    std::cout << "  Table Name: STUDENTS (" << DUMMY_DATA.size() << " rows)\n";
    std::cout << "  Plaintext Dataset:\n";
    printPlaintextTable(DUMMY_DATA);

    auto t_enc_start = std::chrono::high_resolution_clock::now();

    // Prepare column vectors
    std::vector<int64_t> ids, ages, scores;
    ids.reserve(DUMMY_DATA.size());
    ages.reserve(DUMMY_DATA.size());
    scores.reserve(DUMMY_DATA.size());

    for (auto const& r : DUMMY_DATA) {
        ids.push_back(r.id);
        ages.push_back(r.age);
        scores.push_back(r.score);
    }

    // Encrypt columns with BFV vector packing
    crypto::BFVEncryption bfv(harness.encryption->getContext(), harness.encryption->getKeyManager());
    auto ct_id    = bfv.encryptIntegerVector(ids);
    auto ct_age   = bfv.encryptIntegerVector(ages);
    auto ct_score = bfv.encryptIntegerVector(scores);

    auto* ofhe_id    = dynamic_cast<crypto::OpenFHECiphertext*>(ct_id.get());
    auto* ofhe_age   = dynamic_cast<crypto::OpenFHECiphertext*>(ct_age.get());
    auto* ofhe_score = dynamic_cast<crypto::OpenFHECiphertext*>(ct_score.get());
    assert(ofhe_id && ofhe_age && ofhe_score);

    // Register in in-memory storage manager
    harness.storage = std::make_shared<test::InMemoryStorageManager>();
    harness.storage->addTable(
        "STUDENTS",
        {"id", "age", "score"},
        {*ofhe_id, *ofhe_age, *ofhe_score},
        DUMMY_DATA.size()
    );

    // Register in catalog
    std::vector<catalog::ColumnSchema> columns = {
        {"id",    types::DataType::Integer, false, 0},
        {"age",   types::DataType::Integer, false, 1},
        {"score", types::DataType::Integer, false, 2}
    };
    catalog::TableSchema students_schema("STUDENTS", std::move(columns));
    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("STUDENTS", std::move(students_schema));

    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables));
    harness.catalog = std::make_shared<catalog::InMemoryCatalog>(snapshot);

    auto t_enc_end = std::chrono::high_resolution_clock::now();
    double enc_ms = std::chrono::duration<double, std::milli>(t_enc_end - t_enc_start).count();

    std::cout << "  ✓ Columns 'id', 'age', 'score' encrypted and registered in "
              << std::fixed << std::setprecision(2) << enc_ms << " ms\n";

    return harness;
}

bool runLogicalQueryTest(
    TestHarness& harness,
    std::string const& test_title,
    std::string const& sql,
    std::vector<int64_t> const& expected_matched_ids)
{
    std::cout << "\n------------------------------------------------------------------------\n";
    std::cout << " TEST: " << test_title << "\n";
    std::cout << " SQL:  " << sql << "\n";
    std::cout << "------------------------------------------------------------------------\n";

    auto t_total_start = std::chrono::high_resolution_clock::now();

    // 1. Client Parse & Validate
    auto t_parse_start = std::chrono::high_resolution_clock::now();
    client::SQLParser parser;
    auto ast = parser.parseToAST(sql);
    if (!ast) {
        std::cerr << "  ✗ Parse failed for: " << sql << "\n";
        return false;
    }
    auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
    if (!select_stmt) {
        std::cerr << "  ✗ Expected SelectStmt\n";
        return false;
    }

    client::semantic::SemanticValidator validator(*harness.catalog);
    try {
        validator.validate(*select_stmt);
    } catch (client::semantic::SemanticError const& e) {
        std::cerr << "  ✗ Semantic validation failed: " << e.what() << "\n";
        return false;
    }
    auto t_parse_end = std::chrono::high_resolution_clock::now();
    double parse_ms = std::chrono::duration<double, std::milli>(t_parse_end - t_parse_start).count();

    // Extract projection columns
    auto* table_schema = harness.catalog->findTable("STUDENTS");
    std::vector<std::string>     proj_col_names;
    std::vector<types::DataType> proj_col_types;
    for (auto const& p : select_stmt->projections()) {
        if (p->kind() == client::ast::ASTNodeKind::Wildcard) {
            for (auto const& c : table_schema->columns()) {
                proj_col_names.push_back(c.name);
                proj_col_types.push_back(c.type);
            }
            break;
        } else if (p->kind() == client::ast::ASTNodeKind::ColumnRef) {
            auto* cr = dynamic_cast<client::ast::ColumnRef const*>(p.get());
            auto* cs = table_schema->findColumn(cr->name());
            if (cs) {
                proj_col_names.push_back(cs->name);
                proj_col_types.push_back(cs->type);
            }
        }
    }

    // 2. Client Query Preparation (Parameter Encryption)
    auto t_prep_start = std::chrono::high_resolution_clock::now();
    client::preparation::QueryPreparer preparer(*harness.encryption);
    auto prepared = preparer.prepare(std::move(ast));
    auto t_prep_end = std::chrono::high_resolution_clock::now();
    double prep_ms = std::chrono::duration<double, std::milli>(t_prep_end - t_prep_start).count();

    // 3. Protocol Message Encoding
    auto t_enc_start = std::chrono::high_resolution_clock::now();
    auto request = client::protocol::ExecuteQueryEncoder::encode(
        prepared, "query-" + test_title);
    auto t_enc_end = std::chrono::high_resolution_clock::now();
    double enc_ms = std::chrono::duration<double, std::milli>(t_enc_end - t_enc_start).count();

    // 4. Server Execution
    auto t_exec_start = std::chrono::high_resolution_clock::now();
    server::QueryExecutor executor(harness.catalog, harness.storage, harness.crypto_context);
    auto result = executor.execute(request);
    auto t_exec_end = std::chrono::high_resolution_clock::now();
    double exec_ms = std::chrono::duration<double, std::milli>(t_exec_end - t_exec_start).count();

    // 5. Client Decryption
    auto t_dec_start = std::chrono::high_resolution_clock::now();
    execution::ResultDecryptor decryptor(harness.encryption);
    auto decrypted = decryptor.decrypt(result, proj_col_names, proj_col_types);
    auto t_dec_end = std::chrono::high_resolution_clock::now();
    double dec_ms = std::chrono::duration<double, std::milli>(t_dec_end - t_dec_start).count();

    auto t_total_end = std::chrono::high_resolution_clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(t_total_end - t_total_start).count();

    // Log operation timings
    std::cout << "  [TIMING] Client Parse & Validate:    " << std::setw(8) << std::fixed << std::setprecision(2) << parse_ms << " ms\n";
    std::cout << "  [TIMING] Client Query Preparation:   " << std::setw(8) << std::fixed << std::setprecision(2) << prep_ms  << " ms\n";
    std::cout << "  [TIMING] Request Protocol Encoding:  " << std::setw(8) << std::fixed << std::setprecision(2) << enc_ms   << " ms\n";
    std::cout << "  [TIMING] Server Query Execution:     " << std::setw(8) << std::fixed << std::setprecision(2) << exec_ms  << " ms\n";
    std::cout << "  [TIMING] Client Result Decryption:   " << std::setw(8) << std::fixed << std::setprecision(2) << dec_ms   << " ms\n";
    std::cout << "  [TIMING] Total Operation Time:       " << std::setw(8) << std::fixed << std::setprecision(2) << total_ms << " ms\n";

    // Print Results
    std::cout << "\n  Query Results (" << decrypted.rows.size() << " rows scanned):\n";
    std::vector<int64_t> actual_matched_ids;

    for (size_t i = 0; i < decrypted.rows.size(); ++i) {
        auto const& row = decrypted.rows[i];
        int64_t row_id = std::stoll(row.values[0]);

        std::string status = row.matches_filter ? "[MATCH]       " : "[FILTERED OUT]";
        if (row.matches_filter) {
            actual_matched_ids.push_back(row_id);
        }

        std::cout << "    " << status << " Row " << (i + 1) << ": ";
        for (size_t c = 0; c < proj_col_names.size(); ++c) {
            std::cout << proj_col_names[c] << "=" << row.values[c];
            if (c + 1 < proj_col_names.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    // Verify correctness
    std::sort(actual_matched_ids.begin(), actual_matched_ids.end());
    auto sorted_expected = expected_matched_ids;
    std::sort(sorted_expected.begin(), sorted_expected.end());

    bool passed = (actual_matched_ids == sorted_expected);

    std::cout << "\n  Verification:\n";
    std::cout << "    Expected matched IDs: { ";
    for (auto id : sorted_expected) std::cout << id << " ";
    std::cout << "}\n";
    std::cout << "    Actual matched IDs:   { ";
    for (auto id : actual_matched_ids) std::cout << id << " ";
    std::cout << "}\n";
    std::cout << "    Status: " << (passed ? "✓ PASS" : "✗ FAIL") << "\n";

    return passed;
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        HEGA-DB Logical Operations Integration Test (AND & OR)        ║\n";
    std::cout << "║         End-to-end evaluation over encrypted in-memory data          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";

    TestHarness harness = setupDummyTable();

    int total_tests = 0;
    int passed_tests = 0;

    auto run_test = [&](std::string const& title, std::string const& sql, std::vector<int64_t> const& expected) {
        total_tests++;
        if (runLogicalQueryTest(harness, title, sql, expected)) {
            passed_tests++;
        }
    };

    // -------------------------------------------------------------------------
    // Test 1: Simple Equality AND
    // Matches row 2 (id=2, age=22, score=90). Row 6 has age=22, score=75 -> filtered out!
    // -------------------------------------------------------------------------
    run_test(
        "1. Equality AND (age = 22 AND score = 90)",
        "SELECT id, age, score FROM STUDENTS WHERE age = 22 AND score = 90",
        {2}
    );

    // -------------------------------------------------------------------------
    // Test 2: Range AND with Inequalities
    // Matches rows with age in [22, 30]: ids 2 (22), 3 (25), 4 (28), 5 (30), 6 (22)
    // -------------------------------------------------------------------------
    run_test(
        "2. Range AND (age >= 22 AND age <= 30)",
        "SELECT id, age, score FROM STUDENTS WHERE age >= 22 AND age <= 30",
        {2, 3, 4, 5, 6}
    );

    // -------------------------------------------------------------------------
    // Test 3: Simple OR
    // Matches row 3 (age=25) and row 4 (score=95): ids 3, 4
    // -------------------------------------------------------------------------
    run_test(
        "3. Simple OR (age = 25 OR score = 95)",
        "SELECT id, age, score FROM STUDENTS WHERE age = 25 OR score = 95",
        {3, 4}
    );

    // -------------------------------------------------------------------------
    // Test 4: Multiple Disjoint OR
    // Matches rows with id = 1, 5, 8
    // -------------------------------------------------------------------------
    run_test(
        "4. Multiple Disjoint OR (id = 1 OR id = 5 OR id = 8)",
        "SELECT id, age, score FROM STUDENTS WHERE id = 1 OR id = 5 OR id = 8",
        {1, 5, 8}
    );

    // -------------------------------------------------------------------------
    // Test 5: Combined Nested (AND) OR
    // (age = 20 AND score >= 90) OR age = 35
    // Row 1 (id=1, age=20, score=85): score < 90 -> false
    // Row 7 (id=7, age=35, score=88): age=35 -> MATCH
    // Row 8 (id=8, age=20, score=92): age=20 AND score=92 >= 90 -> MATCH
    // Expected matched IDs: {7, 8}
    // -------------------------------------------------------------------------
    run_test(
        "5. Combined (AND) OR: ((age = 20 AND score >= 90) OR age = 35)",
        "SELECT id, age, score FROM STUDENTS WHERE (age = 20 AND score >= 90) OR age = 35",
        {7, 8}
    );

    // -------------------------------------------------------------------------
    // Summary
    // -------------------------------------------------------------------------
    std::cout << "\n========================================================================\n";
    std::cout << "  FINAL SUMMARY: " << passed_tests << " / " << total_tests << " TESTS PASSED\n";
    std::cout << "========================================================================\n";

    if (passed_tests == total_tests) {
        std::cout << "  ✓ All AND and OR logical operations evaluated accurately on encrypted data!\n\n";
        return 0;
    } else {
        std::cerr << "  ✗ Some tests failed! Review the logs above.\n\n";
        return 1;
    }
}
