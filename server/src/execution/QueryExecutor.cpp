#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/execution/ExecutionEngine.h>
#include <hegadb/server/planner/QueryPlanner.h>
#include <hegadb/transport/types/CatalogTypes.h>
#include <stdexcept>
#include <unordered_map>

namespace hegadb::server {

namespace {

class DelegatingCatalog : public catalog::Catalog {
public:
    explicit DelegatingCatalog(std::shared_ptr<catalog::Catalog const> base)
        : base_(std::move(base)) {}

    catalog::SchemaVersion version() const override {
        return base_ ? base_->version() : 1;
    }

    catalog::TableSchema const* findTable(std::string const& name) const override {
        auto it = dynamic_tables_.find(name);
        if (it != dynamic_tables_.end()) {
            return &it->second;
        }
        return base_ ? base_->findTable(name) : nullptr;
    }

    void addTable(catalog::TableSchema table) {
        dynamic_tables_.insert_or_assign(table.name(), std::move(table));
    }

private:
    std::shared_ptr<catalog::Catalog const> base_;
    std::unordered_map<std::string, catalog::TableSchema> dynamic_tables_;
};

} // namespace

QueryExecutor::QueryExecutor(
    std::shared_ptr<catalog::Catalog const> catalog,
    std::shared_ptr<storage::StorageManager> storage,
    std::shared_ptr<crypto::OpenFHEContext> crypto_context,
    std::shared_ptr<transport::CatalogMutationClientTransport> catalog_mutation)
    : catalog_(std::make_shared<DelegatingCatalog>(std::move(catalog)))
    , storage_(std::move(storage))
    , crypto_context_(std::move(crypto_context))
    , catalog_mutation_(std::move(catalog_mutation)) {
}

execution::ExecutionResult QueryExecutor::executeCreateTable(
    protocol::CreateTableStatement const& stmt)
{
    hegadb::logging::info("[EXECUTOR] Executing CREATE TABLE '{}'", stmt.table());

    std::vector<catalog::ColumnSchema> columns;
    columns.reserve(stmt.columns_size());
    for (int i = 0; i < stmt.columns_size(); ++i) {
        auto const& c = stmt.columns(i);
        types::DataType dt = types::DataType::Integer;
        switch (c.data_type()) {
        case protocol::DATA_TYPE_INTEGER: dt = types::DataType::Integer; break;
        case protocol::DATA_TYPE_FLOAT:   dt = types::DataType::Float; break;
        case protocol::DATA_TYPE_STRING:  dt = types::DataType::String; break;
        case protocol::DATA_TYPE_BOOLEAN: dt = types::DataType::Boolean; break;
        default: break;
        }
        columns.push_back(catalog::ColumnSchema{
            .name = c.name(),
            .type = dt,
            .nullable = c.nullable(),
            .ordinal = static_cast<std::size_t>(i)
        });
    }

    catalog::TableSchema schema(stmt.table(), std::move(columns));

    if (!storage_->createTable(schema)) {
        hegadb::logging::warn("[EXECUTOR] StorageManager does not support createTable for '{}'", stmt.table());
        throw std::runtime_error(
            "CREATE TABLE '" + stmt.table()
            + "' is not implemented on this server");
    }

    if (catalog_mutation_) {
        transport::CatalogTable cat_table;
        cat_table.name = stmt.table();
        for (int i = 0; i < stmt.columns_size(); ++i) {
            auto const& c = stmt.columns(i);
            std::string dt_str = "INTEGER";
            switch (c.data_type()) {
            case protocol::DATA_TYPE_INTEGER: dt_str = "INTEGER"; break;
            case protocol::DATA_TYPE_FLOAT:   dt_str = "FLOAT"; break;
            case protocol::DATA_TYPE_STRING:  dt_str = "STRING"; break;
            case protocol::DATA_TYPE_BOOLEAN: dt_str = "BOOLEAN"; break;
            default: break;
            }
            cat_table.columns.push_back(transport::CatalogColumn{
                .name = c.name(),
                .data_type = dt_str,
                .nullable = c.nullable(),
                .ordinal = static_cast<uint64_t>(i)
            });
        }
        try {
            catalog_mutation_->createTable(std::move(cat_table));
            hegadb::logging::info("[EXECUTOR] Table '{}' registered with catalog microservice", stmt.table());
        } catch (std::exception const& e) {
            hegadb::logging::warn("[EXECUTOR] Catalog mutation RPC warning: {}", e.what());
        }
    }

    updateLocalCatalog(schema);
    hegadb::logging::info("[EXECUTOR] CREATE TABLE '{}' completed successfully", stmt.table());

    execution::ExecutionResult result;
    result.setResultMode(protocol::RESULT_MODE_ROWS_AND_MASK);
    return result;
}

execution::ExecutionResult QueryExecutor::executeInsert(
    protocol::InsertStatement const& stmt,
    protocol::ExecuteQuery const& request)
{
    hegadb::logging::info("[EXECUTOR] Executing INSERT into table '{}'", stmt.table());

    auto const* table_schema = catalog_->findTable(stmt.table());
    if (!table_schema) {
        hegadb::logging::error("[EXECUTOR] INSERT error: table '{}' not found", stmt.table());
        throw std::runtime_error("Table not found: " + stmt.table());
    }

    // Determine target columns
    std::vector<std::string> target_columns;
    if (stmt.columns_size() > 0) {
        for (int i = 0; i < stmt.columns_size(); ++i) {
            target_columns.push_back(stmt.columns(i));
        }
    } else {
        for (auto const& col : table_schema->columns()) {
            target_columns.push_back(col.name);
        }
    }

    // Map parameters
    std::unordered_map<uint32_t, std::string> param_ciphertexts;
    for (auto const& p : request.parameters()) {
        param_ciphertexts[p.index()] = p.ciphertext();
    }

    std::unordered_map<std::string, crypto::OpenFHECiphertext> column_values;

    for (size_t i = 0; i < target_columns.size(); ++i) {
        auto const& col_name = target_columns[i];
        auto const* col_def = table_schema->findColumn(col_name);
        if (!col_def) {
            hegadb::logging::error("[EXECUTOR] Column '{}' not found in table '{}'", col_name, stmt.table());
            throw std::runtime_error("Column not found in table '" + stmt.table() + "': " + col_name);
        }

        uint32_t param_idx = static_cast<uint32_t>(i);
        if (stmt.values_size() > static_cast<int>(i)) {
            param_idx = stmt.values(i).parameter_index();
        }

        auto it = param_ciphertexts.find(param_idx);
        if (it == param_ciphertexts.end()) {
            hegadb::logging::error("[EXECUTOR] Missing parameter at index {} for column '{}'", param_idx, col_name);
            throw std::runtime_error("Missing parameter at index " + std::to_string(param_idx));
        }

        auto ct = crypto::OpenFHECiphertext::deserialize(it->second);
        column_values.emplace(col_name, std::move(ct));
    }

    bool success = storage_->appendRow(stmt.table(), column_values, crypto_context_);
    if (!success) {
        hegadb::logging::error("[EXECUTOR] Failed to append row into table '{}'", stmt.table());
        throw std::runtime_error("Failed to append row into table: " + stmt.table());
    }

    hegadb::logging::info("[EXECUTOR] INSERT into table '{}' completed successfully (1 row appended)", stmt.table());

    execution::ExecutionResult result;
    result.setResultMode(protocol::RESULT_MODE_ROWS_AND_MASK);
    return result;
}

void QueryExecutor::updateLocalCatalog(catalog::TableSchema const& schema)
{
    auto* delegating = const_cast<DelegatingCatalog*>(
        dynamic_cast<DelegatingCatalog const*>(catalog_.get()));
    if (delegating) {
        delegating->addTable(schema);
    }
}

execution::ExecutionResult QueryExecutor::execute(
    protocol::ExecuteQuery const& request) {

    // Validate request
    if (!request.has_statement()) {
        throw std::runtime_error("Request contains no statement");
    }

    auto const& stmt = request.statement();

    if (stmt.has_create_table_statement()) {
        return executeCreateTable(stmt.create_table_statement());
    }

    if (stmt.has_insert_statement()) {
        return executeInsert(stmt.insert_statement(), request);
    }

    if (stmt.has_drop_table_statement()) {
        throw std::runtime_error(
            "DROP TABLE '" + stmt.drop_table_statement().table()
            + "' is not implemented on this server");
    }

    if (!stmt.has_select_statement()) {
        throw std::runtime_error("Unsupported statement type");
    }

    auto const& select_stmt = stmt.select_statement();

    // JOIN — planning is supported but execution is not yet implemented.
    if (select_stmt.joins_size() > 0) {
        auto const& join = select_stmt.joins(0);
        throw std::runtime_error(
            "INNER JOIN between '"
            + select_stmt.from_table().table()
            + "' and '"
            + join.right_table().table()
            + "' is not implemented on this server");
    }

    // Determine result mode
    auto result_mode = (request.result_mode() == protocol::RESULT_MODE_UNSPECIFIED)
        ? protocol::RESULT_MODE_ROWS_AND_MASK
        : request.result_mode();

    // Create execution context
    execution::ExecutionContext context(
        catalog_,
        storage_,
        crypto_context_,
        result_mode
    );

    context.setRequestId(request.request_id().value());

    // Bind parameters
    for (auto const& param : request.parameters()) {
        context.getParameters().addParameter(
            param.index(),
            param.ciphertext()
        );
    }

    // Plan query
    planner::QueryPlanner planner(catalog_);
    auto plan = planner.plan(select_stmt);

    // Execute plan
    execution::ExecutionEngine engine;
    return engine.execute(plan, context);
}

} // namespace hegadb::server
