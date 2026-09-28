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
    if (!request.has_statement() || !request.statement().has_select_statement()) {
        throw std::runtime_error("Only SELECT statements supported");
    }

    auto const& select_stmt = request.statement().select_statement();

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
