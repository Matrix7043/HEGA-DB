/**
 * test_query_execution.cpp
 *
 * Phase 3 integration test: End-to-end query execution with file-backed storage.
 *
 * Flow:
 *  1. Load keys from disk (written by hegadb_setup)
 *  2. Build server components (catalog, FileStorageManager, QueryExecutor)
 *  3. Simulate client: parse SQL -> validate -> prepare -> encode protocol msg
 *  4. Server executes on encrypted EMPLOYEES data from ~/.hegadb/data
 *  5. Client decrypts and prints results
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
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>

// Crypto
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>

// Execution utilities
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

using namespace hegadb;

int main() {
    std::cout << "\n";
    std::cout << "=== HEGA-DB Phase 3: File-Backed Query Execution ===\n\n";

    std::string home_dir = std::getenv("HOME");
    std::string data_dir    = home_dir + "/.hegadb/data";
    std::string keys_dir    = home_dir + "/.hegadb/keys";
    std::string catalog_dir = home_dir + "/.hegadb/catalog";

    // =====================================================================
    // Phase 1: Load encryption — keys were written by hegadb_setup
    // =====================================================================
    std::cout << "Phase 1: Loading Encryption\n";
    std::cout << "  Key directory: " << keys_dir << "\n";

    auto encryption = std::make_shared<crypto::CompositeEncryption>(keys_dir);
    std::cout << "  OK CompositeEncryption loaded (keys from disk)\n";

    // The server needs a shared_ptr<OpenFHEContext>.
    // CompositeEncryption owns its context internally; wire a new wrapper
    // that shares the same already-initialised BFV/CKKS contexts.
    auto crypto_context = std::make_shared<crypto::OpenFHEContext>();
    crypto_context->setBFVContext(encryption->getContext().getBFVContext());
    crypto_context->setCKKSContext(encryption->getContext().getCKKSContext());
    std::cout << "  OK OpenFHEContext wired from CompositeEncryption\n\n";

    // =====================================================================
    // Phase 2: Server — catalog + file storage + executor
    // =====================================================================
    std::cout << "Phase 2: Server Setup\n";

    // Build the EMPLOYEES catalog entry (same schema as hegadb_setup created).
    std::vector<catalog::ColumnSchema> columns = {
        {"id",     types::DataType::Integer, false, 0},
        {"age",    types::DataType::Integer, false, 1},
        {"salary", types::DataType::Integer, false, 2},
        {"dept",   types::DataType::String,  false, 3},
    };
    catalog::TableSchema employees_schema("EMPLOYEES", std::move(columns));

    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("EMPLOYEES", std::move(employees_schema));

    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables));
    auto catalog  = std::make_shared<catalog::InMemoryCatalog>(snapshot);
    std::cout << "  OK Catalog built (EMPLOYEES: id, age, salary, dept)\n";

    std::shared_ptr<server::storage::StorageManager> storage;
    try {
        storage = server::storage::StorageManager::createFileStorage(data_dir, catalog_dir);
        std::cout << "  OK FileStorageManager: " << data_dir << "\n";
    } catch (std::exception const& e) {
        std::cerr << "  FAIL Storage init: " << e.what() << "\n";
        return 1;
    }

    server::QueryExecutor executor(catalog, storage, crypto_context);
    std::cout << "  OK QueryExecutor ready\n\n";

    // =====================================================================
    // Phase 3: Client — parse, validate, prepare, encode
    // =====================================================================
    std::cout << "Phase 3: Client - Preparing Query\n";

    std::string sql = "SELECT * FROM EMPLOYEES";
    std::cout << "  SQL: " << sql << "\n";

    // Parse
    client::SQLParser parser;
    auto ast = parser.parseToAST(sql);
    if (!ast) {
        std::cerr << "  FAIL Parse failed\n";
        return 1;
    }
    std::cout << "  OK Parsed\n";

    auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
    if (!select_stmt) {
        std::cerr << "  FAIL Only SELECT statements supported\n";
        return 1;
    }

    // Semantic validation
    client::semantic::SemanticValidator validator(*catalog);
    try {
        validator.validate(*select_stmt);
        std::cout << "  OK Semantic validation\n";
    } catch (client::semantic::SemanticError const& e) {
        std::cerr << "  FAIL Semantic error: " << e.what() << "\n";
        return 1;
    }

    // Determine projected columns for later decryption
    // (mirrors the logic in integrated_query_test.cpp)
    std::vector<std::string>     col_names;
    std::vector<types::DataType> col_types;

    auto* table_schema = catalog->findTable(select_stmt->from()->name());
    if (!table_schema) {
        std::cerr << "  FAIL Table schema not found\n";
        return 1;
    }

    bool has_wildcard = false;
    for (auto const& proj : select_stmt->projections()) {
        if (proj->kind() == client::ast::ASTNodeKind::Wildcard) {
            has_wildcard = true;
            break;
        }
    }

    if (has_wildcard) {
        for (auto const& col : table_schema->columns()) {
            col_names.push_back(col.name);
            col_types.push_back(col.type);
        }
    } else {
        for (auto const& proj : select_stmt->projections()) {
            if (proj->kind() == client::ast::ASTNodeKind::ColumnRef) {
                auto* col_ref = dynamic_cast<client::ast::ColumnRef const*>(proj.get());
                if (col_ref) {
                    auto* col_schema = table_schema->findColumn(col_ref->name());
                    if (col_schema) {
                        col_names.push_back(col_schema->name);
                        col_types.push_back(col_schema->type);
                    }
                }
            }
        }
    }
    std::cout << "  OK Projected " << col_names.size() << " column(s): ";
    for (size_t i = 0; i < col_names.size(); ++i) {
        std::cout << col_names[i];
        if (i + 1 < col_names.size()) std::cout << ", ";
    }
    std::cout << "\n";

    // Prepare (encrypt any literal parameters — none for SELECT *)
    client::preparation::QueryPreparer preparer(*encryption);
    auto prepared = preparer.prepare(std::move(ast));
    std::cout << "  OK Prepared (" << prepared.parameters().size() << " parameters)\n";

    // Encode to protocol message
    auto request = client::protocol::ExecuteQueryEncoder::encode(
        prepared,
        "test-query-file-storage"
    );
    std::cout << "  OK Protocol message encoded\n\n";

    // =====================================================================
    // Phase 4: Server executes on encrypted data
    // =====================================================================
    std::cout << "Phase 4: Server - Executing Query on Encrypted Data\n";

    server::execution::ExecutionResult result;
    try {
        result = executor.execute(request);
        std::cout << "  OK Executed — " << result.getBatches().size()
                  << " batch(es), " << result.getTotalRowCount() << " row(s)\n\n";
    } catch (std::exception const& e) {
        std::cerr << "  FAIL Execution: " << e.what() << "\n";
        return 1;
    }

    if (result.getBatches().empty()) {
        std::cerr << "  FAIL No result batches returned\n";
        return 1;
    }

    // =====================================================================
    // Phase 5: Client decrypts and displays results
    // =====================================================================
    std::cout << "Phase 5: Client - Decrypting Results\n";

    execution::ResultDecryptor decryptor(encryption);
    execution::DecryptedResult decrypted;
    try {
        decrypted = decryptor.decrypt(result, col_names, col_types);
        std::cout << "  OK Decrypted " << decrypted.rows.size() << " row(s)\n\n";
    } catch (std::exception const& e) {
        std::cerr << "  FAIL Decryption: " << e.what() << "\n";
        return 1;
    }

    // Display
    execution::FormatConfig fmt_config;
    fmt_config.format = execution::DisplayFormat::Table;
    execution::ResultFormatter formatter(fmt_config);
    formatter.format(decrypted);

    std::cout << "\n=== Test completed successfully ===\n\n";
    return 0;
}
