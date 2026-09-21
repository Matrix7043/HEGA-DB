#include <hegadb/server/ExecuteQueryReceiver.h>

namespace hegadb::server {

int constexpr SUPPORTED_MAJOR_VERSION = 1;
int constexpr SUPPORTED_MINOR_VERSION = 1;

ExecuteQueryReceiver::ExecuteQueryReceiver(
    // std::shared_ptr<catalog::Catalog const> catalog,
    // std::shared_ptr<QueryExecutor> executor
    )
    : catalog_()
    , executor_()
{
}

protocol::QueryResponse ExecuteQueryReceiver::executeQuery(
    protocol::ExecuteQuery const& request)
{
    std::string error_message;

    // Validate protocol_version
    if (!validateProtocolVersion(request.protocol_version(), error_message)) {
        return createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_ERROR,
            "Protocol version mismatch: " + error_message);
    }

    // Execute
    try {
        auto const& select = request.statement().select_statement();

        // TODO : Pass SelectStatement and parameters array to QueryExecutor
        //  QueryResult result = executor_->execute(
        //      select,
        //      request.parameters());

        // if (!result.success) {
        if (false) {
            return createResponse(
                request.protocol_version(),
                request.request_id(),
                protocol::STATUS_ERROR,
                "Execution failed: ");
        }

        // Stage 4: Construct Success Response
        return createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_OK,
            "Query executed successfully. Rows processed: ");
    } catch (std::exception const& ex) {
        return createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_ERROR,
            std::string("Internal server exception during execution: ") + ex.what());
    }
}

bool ExecuteQueryReceiver::validateProtocolVersion(
    protocol::ProtocolVersion const& version,
    std::string& error_message) const
{
    if (version.major() != SUPPORTED_MAJOR_VERSION || version.minor() != SUPPORTED_MINOR_VERSION) {
        error_message = "Client version " + std::to_string(version.major()) + "." + std::to_string(version.minor()) + " is unsupported. Server supports " + std::to_string(SUPPORTED_MAJOR_VERSION) + "." + std::to_string(SUPPORTED_MINOR_VERSION);
        return false;
    }
    return true;
}

protocol::QueryResponse ExecuteQueryReceiver::createResponse(
    protocol::ProtocolVersion const& version,
    protocol::RequestId const& request_id,
    protocol::StatusCode code,
    std::string message) const
{
    protocol::QueryResponse response;
    *response.mutable_protocol_version() = version;
    *response.mutable_request_id() = request_id;
    response.mutable_status()->set_code(code);
    response.mutable_status()->set_message(std::move(message));
    return response;
}

} // namespace hegadb::server
