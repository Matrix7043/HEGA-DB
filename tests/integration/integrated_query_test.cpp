#include "InMemoryStorageManager.h"
#include "TestDataGenerator.h"

// Client components
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/Wildcard.h>

// Server components
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/planner/QueryPlanner.h>

// Catalog and crypto
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>

// Execution utilities
#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/execution/ResultFormatter.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>

using namespace hegadb;

/**
 * @brief Integrated Query Test System
 * 
 * Tests the complete query execution pipeline:
 * 1. Setup: Create encrypted database in memory
 * 2. Loop: Accept SQL queries, execute on encrypted data, decrypt results
 * 3. Display: Pretty-print results
 */
class IntegratedQueryTest {
public:
    IntegratedQueryTest()
        : encryption_(std::make_shared<crypto::CompositeEncryption>())
        , crypto_context_(std::make_shared<crypto::OpenFHEContext>())
        , storage_(std::make_shared<test::InMemoryStorageManager>())
        , catalog_(std::make_shared<catalog::InMemoryCatalog>(createEmptyCatalogSnapshot()))
    {
    }

    int run() {
        displayBanner();
        
        if (!setup()) {
            std::cerr << "[ERROR] Setup failed\n";
            return 1;
        }

        queryLoop();
        
        return 0;
    }

private:
    static std::shared_ptr<catalog::CatalogSnapshot const> createEmptyCatalogSnapshot() {
        std::unordered_map<std::string, catalog::TableSchema> tables;
        return std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables));
    }

    void displayBanner() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════╗\n";
        std::cout << "║   HEGA-DB Integrated Query Execution Test         ║\n";
        std::cout << "║   End-to-End Encrypted Query Testing              ║\n";
        std::cout << "╚════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
    }

    bool setup() {
        std::cout << "=== SETUP PHASE ===\n\n";

        try {
            // Initialize crypto context
            std::cout << "[SETUP] Initializing cryptographic context...\n";
            crypto_context_->enableFeatures();
            std::cout << "[SETUP]   ✓ OpenFHE context initialized\n";

            // Generate test data
            std::cout << "\n";
            test::TestDataGenerator generator(encryption_);
            generator.generateStudentsTable(*storage_, catalog_);

            std::cout << "\n[SETUP] ✓ Database ready!\n";
            std::cout << "\n=== READY FOR QUERIES ===\n\n";

            return true;

        } catch (std::exception const& e) {
            std::cerr << "[SETUP] ERROR: " << e.what() << "\n";
            return false;
        }
    }

    void queryLoop() {
        std::cout << "Type SQL queries to execute on encrypted data.\n";
        std::cout << "Commands:\n";
        std::cout << "  \\schema STUDENTS  - Show table schema\n";
        std::cout << "  \\q, \\quit, exit   - Exit\n";
        std::cout << "\n";

        using_history();

        while (true) {
            char* raw_input = readline("hegadb-test> ");

            if (raw_input == nullptr) {
                std::cout << "\n";
                break;
            }

            std::string input(raw_input);
            free(raw_input);

            if (input.empty()) {
                continue;
            }

            std::cout << "[LOOP]     Got input: " << input << std::endl;  // force flush

            if (input == "\\q" || input == "\\quit" || input == "exit") {
                break;
            }

            std::cout << "[LOOP]     Adding to history" << std::endl;
            add_history(input.c_str());
            std::cout << "[LOOP]     Added to history" << std::endl;

            std::cout << "[LOOP]     Checking for schema command" << std::endl;
            if (input.find("\\schema") == 0) {
                displaySchema();
                continue;
            }

            std::cout << "[MAIN]     About to call executeQuery" << std::endl;
            executeQuery(input);
            std::cout << "[MAIN]     Returned from executeQuery" << std::endl;
        }

        std::cout << "Goodbye!\n";
    }

    void displaySchema() {
        std::cout << "\nTable: STUDENTS\n";
        std::cout << "Columns:\n";
        std::cout << "  NAME      STRING\n";
        std::cout << "  CLASS     INTEGER\n";
        std::cout << "  GRADE     INTEGER\n";
        std::cout << "\n";
    }

    void executeQuery(std::string const& sql) {
        std::cout << "[DEBUG]    executeQuery called" << std::endl;
        std::cout << "[DEBUG]    encryption_ = " << (encryption_ ? "valid" : "null") << std::endl;
        std::cout << "[DEBUG]    catalog_ = " << (catalog_ ? "valid" : "null") << std::endl;
        std::cout << "[DEBUG]    sql = " << sql << std::endl;
        std::cout << "[DEBUG]    About to enter try block" << std::endl;

        try {
            std::cout << "[DEBUG]    Inside try block" << std::endl;
            auto start_time = std::chrono::high_resolution_clock::now();
            std::cout << "[DEBUG]    Got start time" << std::endl;

            // ========================================
            // CLIENT SIDE: Parse and Prepare
            // ========================================
            
            std::cout << "[PARSE]    Parsing SQL...\n";
            std::cout << "[DEBUG]    Creating SQLParser" << std::endl;
            client::SQLParser parser;
            std::cout << "[DEBUG]    Calling parseToAST" << std::endl;
            auto ast = parser.parseToAST(sql);
            std::cout << "[DEBUG]    parseToAST returned" << std::endl;

            std::cout << "[DEBUG]    Checking if ast is null" << std::endl;
            if (!ast) {
                std::cout << "[PARSE]    ✗ Parse failed\n";
                return;
            }
            std::cout << "[DEBUG]    ast is valid" << std::endl;
            std::cout << "[PARSE]    SQL parsed successfully" << std::endl;

            std::cout << "[DEBUG]    Casting to SelectStmt" << std::endl;
            auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
            std::cout << "[DEBUG]    Cast complete, checking if null" << std::endl;
            if (!select_stmt) {
                std::cout << "[ERROR]    Only SELECT statements supported\n";
                return;
            }
            std::cout << "[DEBUG]    SELECT statement is valid" << std::endl;

            // Validate
            std::cout << "[VALIDATE] Checking semantics...\n";
            client::semantic::SemanticValidator validator(*catalog_);
            
            try {
                validator.validate(*select_stmt);
                std::cout << "[VALIDATE] ✓ Semantic validation passed\n";
            } catch (client::semantic::SemanticError const& e) {
                std::cout << "[VALIDATE] ✗ " << e.what() << "\n";
                return;
            }

            // Prepare (encrypt parameters)
            std::cout << "[PREPARE]  Encrypting parameters...\n";
            client::preparation::QueryPreparer preparer(*encryption_);
            auto prepared = preparer.prepare(std::move(ast));
            std::cout << "[PREPARE]  ✓ Encrypted " << prepared.parameters().size() << " parameter(s)\n";

            // Encode to protocol
            auto request = client::protocol::ExecuteQueryEncoder::encode(
                prepared,
                "test-query-" + std::to_string(std::time(nullptr))
            );

            // ========================================
            // SERVER SIDE: Execute on Encrypted Data
            // ========================================

            std::cout << "[EXECUTE]  Planning query...\n";
            
            try {
                server::QueryExecutor executor(
                    catalog_,
                    storage_,
                    crypto_context_
                );

                std::cout << "[EXECUTE]  Executing on encrypted data...\n";
                std::cout << "[EXECUTE]  DEBUG: About to call execute...\n";
                auto result = executor.execute(request);
                std::cout << "[EXECUTE]  DEBUG: Execute returned\n";
                std::cout << "[EXECUTE]  ✓ Query executed\n";

                // ========================================
                // CLIENT SIDE: Decrypt Results
                // ========================================

                std::cout << "[DECRYPT]  Preparing to decrypt results...\n";

                // Determine columns
                std::string table_name = select_stmt->from()->name();
                auto* table_schema = catalog_->findTable(table_name);

                std::vector<std::string> column_names;
                std::vector<types::DataType> column_types;

                bool has_wildcard = false;
                for (auto const& proj : select_stmt->projections()) {
                    if (proj->kind() == client::ast::ASTNodeKind::Wildcard) {
                        has_wildcard = true;
                        break;
                    }
                }

                if (has_wildcard) {
                    for (auto const& col : table_schema->columns()) {
                        column_names.push_back(col.name);
                        column_types.push_back(col.type);
                    }
                } else {
                    for (auto const& proj : select_stmt->projections()) {
                        if (proj->kind() == client::ast::ASTNodeKind::ColumnRef) {
                            auto* col_ref = dynamic_cast<client::ast::ColumnRef const*>(proj.get());
                            if (col_ref) {
                                auto* col_schema = table_schema->findColumn(col_ref->name());
                                if (col_schema) {
                                    column_names.push_back(col_schema->name);
                                    column_types.push_back(col_schema->type);
                                }
                            }
                        }
                    }
                }

                // TEMPORARY: Skip decryption for debugging
                execution::DecryptedResult decrypted;
                decrypted.column_names = column_names;
                decrypted.column_types = column_types;
                decrypted.total_rows_scanned = result.getTotalRowCount();

                std::cout << "[DECRYPT]  Result has " << result.getBatches().size() << " batches\n";
                std::cout << "[DECRYPT]  Total row count: " << result.getTotalRowCount() << "\n";
                
                auto const& batches = result.getBatches();
                if (!batches.empty()) {
                    std::cout << "[DECRYPT]  First batch has " << batches[0].getRowCount() << " rows\n";
                    std::cout << "[DECRYPT]  First batch has " << batches[0].getColumnCount() << " columns\n";
                    
                    // Create dummy rows for now
                    for (size_t i = 0; i < batches[0].getRowCount(); ++i) {
                        execution::DecryptedRow row;
                        row.matches_filter = true;
                        row.values.resize(column_names.size(), "<encrypted>");
                        decrypted.rows.push_back(row);
                    }
                }

                std::cout << "[DECRYPT]  ✓ Results prepared (decryption skipped for debugging)\n";

                // ========================================
                // Display Results
                // ========================================

                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    end_time - start_time
                ).count();

                std::cout << "\n";
                
                execution::FormatConfig fmt_config;
                fmt_config.format = execution::DisplayFormat::Table;
                execution::ResultFormatter formatter(fmt_config);
                
                formatter.format(decrypted);

                std::cout << "\nExecution time: " << duration << " ms\n";

            } catch (std::exception const& e) {
                std::cout << "[ERROR]    Exception in try block: " << e.what() << "\n";
            }

        } catch (std::exception const& e) {
            std::cout << "[ERROR]    Outer exception: " << e.what() << "\n";
        } catch (...) {
            std::cout << "[ERROR]    Unknown exception caught\n";
        }

        std::cout << "\n";
    }

public:
    std::shared_ptr<crypto::CompositeEncryption> encryption_;
    std::shared_ptr<crypto::OpenFHEContext> crypto_context_;
    std::shared_ptr<test::InMemoryStorageManager> storage_;
    std::shared_ptr<catalog::InMemoryCatalog> catalog_;
};

int main() {
    IntegratedQueryTest test;
    return test.run();
}
