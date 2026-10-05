#include <hegadb/execution/LocalExecutor.h>
#include <hegadb/execution/ResultDecryptor.h>

// Client components
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>

// Client AST
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/Wildcard.h>

// Server components
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/planner/QueryPlanner.h>
#include <hegadb/server/execution/ExecutionEngine.h>

#include <hegadb/logging/Logger.h>

#include <stdexcept>
#include <ctime>

namespace hegadb::execution {

LocalExecutor::LocalExecutor(
    std::shared_ptr<catalog::Catalog> catalog,
    std::shared_ptr<server::storage::StorageManager> storage,
    std::shared_ptr<crypto::CompositeEncryption> encryption)
    : catalog_(std::move(catalog))
    , storage_(std::move(storage))
    , encryption_(std::move(encryption))
    , crypto_context_(std::make_shared<crypto::OpenFHEContext>())
{
    // Initialize crypto context with necessary features
    crypto_context_->enableFeatures();
}

QueryResult LocalExecutor::execute(std::string const& sql) {
    QueryResult result;
    result.success = false;
    result.total_rows_scanned = 0;

    try {
        logging::debug("LocalExecutor: Parsing SQL query");

        // ========================================================
        // Step 1: Parse SQL
        // ========================================================
        client::SQLParser parser;
        auto ast = parser.parseToAST(sql);

        if (!ast) {
            result.error_message = "SQL parsing failed";
            logging::warn("LocalExecutor: {}", result.error_message);
            return result;
        }

        auto* select_stmt = dynamic_cast<client::ast::SelectStmt*>(ast.get());
        if (!select_stmt) {
            result.error_message = "Only SELECT statements are supported";
            logging::warn("LocalExecutor: {}", result.error_message);
            return result;
        }

        logging::debug("LocalExecutor: SQL parsing successful");

        // ========================================================
        // Step 2: Semantic Validation
        // ========================================================
        logging::debug("LocalExecutor: Starting semantic validation");

        client::semantic::SemanticValidator validator(*catalog_);
        
        try {
            validator.validate(*select_stmt);
        } catch (client::semantic::SemanticError const& error) {
            result.error_message = std::string("Semantic error: ") + error.what();
            logging::warn("LocalExecutor: {}", result.error_message);
            return result;
        }

        logging::debug("LocalExecutor: Semantic validation successful");

        // ========================================================
        // Step 3: Query Preparation (Extract and Encrypt Literals)
        // ========================================================
        logging::debug("LocalExecutor: Preparing query (encrypting parameters)");

        client::preparation::QueryPreparer preparer(*encryption_);
        auto prepared = preparer.prepare(std::move(ast));

        logging::debug("LocalExecutor: Query preparation complete, encrypted_parameters={}",
            prepared.parameters().size());

        // ========================================================
        // Step 4: Encode to Protocol Format
        // ========================================================
        logging::debug("LocalExecutor: Encoding query to protocol format");

        auto request = client::protocol::ExecuteQueryEncoder::encode(
            prepared,
            "local-exec-" + std::to_string(std::time(nullptr))
        );

        logging::debug("LocalExecutor: Protocol encoding complete");

        // ========================================================
        // Step 5: Server-Side Query Execution
        // ========================================================
        logging::debug("LocalExecutor: Creating query executor");

        server::QueryExecutor executor(catalog_, storage_, crypto_context_);

        logging::debug("LocalExecutor: Executing query on encrypted data");

        auto execution_result = executor.execute(request);

        logging::debug("LocalExecutor: Query execution complete");

        // ========================================================
        // Step 6: Extract Result Metadata
        // ========================================================

        // Get table schema to determine column types
        std::string table_name = select_stmt->from()->name();
        auto* table_schema = catalog_->findTable(table_name);
        
        if (!table_schema) {
            result.error_message = "Table not found: " + table_name;
            logging::error("LocalExecutor: {}", result.error_message);
            return result;
        }

        // Determine which columns were selected
        std::vector<std::string> selected_columns;
        std::vector<types::DataType> selected_types;

        // Check if wildcard was used
        bool has_wildcard = false;
        for (auto const& proj : select_stmt->projections()) {
            if (proj->kind() == client::ast::ASTNodeKind::Wildcard) {
                has_wildcard = true;
                break;
            }
        }

        if (has_wildcard) {
            // Select all columns from table
            for (auto const& col : table_schema->columns()) {
                selected_columns.push_back(col.name);
                selected_types.push_back(col.type);
            }
        } else {
            // Select specific columns
            for (auto const& proj : select_stmt->projections()) {
                if (proj->kind() == client::ast::ASTNodeKind::ColumnRef) {
                    auto* col_ref = dynamic_cast<client::ast::ColumnRef const*>(proj.get());
                    if (col_ref) {
                        auto* col_schema = table_schema->findColumn(col_ref->name());
                        if (col_schema) {
                            selected_columns.push_back(col_schema->name);
                            selected_types.push_back(col_schema->type);
                        }
                    }
                }
            }
        }

        // ========================================================
        // Step 7: Decrypt Results
        // ========================================================
        logging::debug("LocalExecutor: Decrypting query results");

        ResultDecryptor decryptor(encryption_);
        auto decrypted = decryptor.decrypt(
            execution_result,
            selected_columns,
            selected_types
        );

        logging::debug("LocalExecutor: Decryption complete, rows={}", decrypted.rows.size());

        // ========================================================
        // Step 8: Filter Rows Based on Mask and Format Result
        // ========================================================

        result.column_names = decrypted.column_names;
        result.column_types = decrypted.column_types;
        result.total_rows_scanned = decrypted.total_rows_scanned;

        // Only include rows that matched the filter
        for (auto const& row : decrypted.rows) {
            if (row.matches_filter) {
                result.rows.push_back(row.values);
            }
        }

        result.success = true;

        logging::info("LocalExecutor: Query successful, matched_rows={}, scanned_rows={}",
            result.rows.size(), result.total_rows_scanned);

    } catch (std::exception const& error) {
        result.error_message = std::string("Execution error: ") + error.what();
        result.success = false;
        logging::error("LocalExecutor: {}", result.error_message);
    }

    return result;
}

catalog::Catalog& LocalExecutor::getCatalog() {
    return *catalog_;
}

catalog::Catalog const& LocalExecutor::getCatalog() const {
    return *catalog_;
}

server::storage::StorageManager& LocalExecutor::getStorageManager() {
    return *storage_;
}

crypto::CompositeEncryption& LocalExecutor::getEncryption() {
    return *encryption_;
}

} // namespace hegadb::execution
