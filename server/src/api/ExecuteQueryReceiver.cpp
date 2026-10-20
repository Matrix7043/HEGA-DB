#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/result/ResultEncoder.h>

namespace hegadb::server {

ExecuteQueryReceiver::ExecuteQueryReceiver(
    std::shared_ptr<catalog::Catalog const> catalog,
    std::shared_ptr<QueryExecutor> executor)
    : catalog_(std::move(catalog))
    , executor_(std::move(executor))
{
}

protocol::QueryResponse ExecuteQueryReceiver::executeQuery(
    protocol::ExecuteQuery const& request)
{

    // Validate protocol version
    std::string error_message;
    if (!validateProtocolVersion(request.protocol_version(), error_message)) {
        return createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_ERROR,
            error_message);
    }

    try {
        // Execute query
        auto exec_result = executor_->execute(request);

        // Create response
        std::string success_msg;
        if (request.has_statement()) {
            if (request.statement().has_create_table_statement()) {
                success_msg = "Table '" + request.statement().create_table_statement().table() + "' created successfully";
            } else if (request.statement().has_insert_statement()) {
                success_msg = "1 row inserted successfully into '" + request.statement().insert_statement().table() + "'";
            }
        }

        auto response = createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_OK,
            success_msg);

        // Encode result
        result::ResultEncoder::encode(exec_result, response);

        return response;
    } catch (std::exception const& e) {
        return createResponse(
            request.protocol_version(),
            request.request_id(),
            protocol::STATUS_ERROR,
            std::string("Execution error: ") + e.what());
    }
}

bool ExecuteQueryReceiver::validateProtocolVersion(
    protocol::ProtocolVersion const& version,
    std::string& error_message) const
{

    // Accept version 1.x
    if (version.major() != 1) {
        error_message = "unsupported protocol version: " + std::to_string(version.major()) + "." + std::to_string(version.minor());
        return false;
    }

    return true;
}

protocol::QueryResponse ExecuteQueryReceiver::createResponse(
    protocol::ProtocolVersion const& version,
    protocol::RequestId const& request_id,
    protocol::StatusCode status,
    std::string message) const
{

    protocol::QueryResponse response;
    *response.mutable_protocol_version() = version;
    *response.mutable_request_id() = request_id;
    response.mutable_status()->set_code(status);
    if (!message.empty()) {
        response.mutable_status()->set_message(message);
    }

    return response;
}

} // namespace hegadb::server
