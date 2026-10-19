#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/execution/ExecutionEngine.h>
#include <hegadb/server/planner/QueryPlanner.h>
#include <stdexcept>

namespace hegadb::server {

QueryExecutor::QueryExecutor(
    std::shared_ptr<catalog::Catalog const> catalog,
    std::shared_ptr<storage::StorageManager> storage,
    std::shared_ptr<crypto::OpenFHEContext> crypto_context)
    : catalog_(std::move(catalog))
    , storage_(std::move(storage))
    , crypto_context_(std::move(crypto_context)) {
}

execution::ExecutionResult QueryExecutor::execute(
    protocol::ExecuteQuery const& request) {

    // Validate request
    if (!request.has_statement()) {
        throw std::runtime_error("Request contains no statement");
    }

    auto const& stmt = request.statement();

    // DDL — not yet implemented; return a descriptive error so callers can
    // distinguish "known but unimplemented" from "unknown request".
    if (stmt.has_create_table_statement()) {
        throw std::runtime_error(
            "CREATE TABLE '" + stmt.create_table_statement().table()
            + "' is not implemented on this server");
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

void QueryExecutor::executeStreaming(
    protocol::ExecuteQuery const& request,
    StreamingBatchConsumer consumer) {

    // Validate request
    if (!request.has_statement()) {
        throw std::runtime_error("Request contains no statement");
    }

    auto const& stmt = request.statement();

    if (stmt.has_create_table_statement()) {
        throw std::runtime_error(
            "CREATE TABLE '" + stmt.create_table_statement().table()
            + "' is not implemented on this server");
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

    auto result_mode = (request.result_mode() == protocol::RESULT_MODE_UNSPECIFIED)
        ? protocol::RESULT_MODE_ROWS_AND_MASK
        : request.result_mode();

    execution::ExecutionContext context(
        catalog_,
        storage_,
        crypto_context_,
        result_mode
    );

    context.setRequestId(request.request_id().value());

    for (auto const& param : request.parameters()) {
        context.getParameters().addParameter(
            param.index(),
            param.ciphertext()
        );
    }

    planner::QueryPlanner planner(catalog_);
    auto plan = planner.plan(select_stmt);

    execution::ExecutionEngine engine;
    engine.executeStreaming(plan, context, [&](execution::ExecutionBatch const& batch, execution::EncryptedMask const* mask) -> bool {
        return consumer(batch, mask, result_mode);
    });
}

} // namespace hegadb::server
