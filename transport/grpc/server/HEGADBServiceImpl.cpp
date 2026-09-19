#include "HEGADBServiceImpl.h"

#include <utility>

namespace hegadb::transport::grpc {

void HEGADBServiceImpl::setRequestHandler(
    RequestHandler handler)
{
    request_handler_ = std::move(handler);
}

::grpc::Status HEGADBServiceImpl::Execute(
    [[maybe_unused]] ::grpc::ServerContext* context,
    ::hegadb::protocol::ExecuteQuery const* request,
    ::hegadb::protocol::QueryResponse* response)
{
    if (request == nullptr || response == nullptr) {
        return ::grpc::Status(
            ::grpc::StatusCode::INVALID_ARGUMENT,
            "Invalid request or response");
    }

    if (!request_handler_) {
        return ::grpc::Status(
            ::grpc::StatusCode::FAILED_PRECONDITION,
            "No request handler configured");
    }

    try {
        *response = request_handler_(*request);

        return ::grpc::Status::OK;
    } catch (std::exception const& ex) {
        return ::grpc::Status(
            ::grpc::StatusCode::INTERNAL,
            ex.what());
    }
}

} // namespace hegadb::transport::grpc
