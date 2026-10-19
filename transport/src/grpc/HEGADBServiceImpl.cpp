#include "hegadb/transport/grpc/HEGADBServiceImpl.h"

#include <utility>

namespace hegadb::transport::grpc {

void HEGADBServiceImpl::setStreamingRequestHandler(
    StreamingRequestHandler handler)
{
    streaming_handler_ = std::move(handler);
}

void HEGADBServiceImpl::setRequestHandler(
    RequestHandler handler)
{
    streaming_handler_ = [h = std::move(handler)](
                             protocol::ExecuteQuery const& req,
                             BatchWriter writer) {
        auto resp = h(req);
        writer(resp);
    };
}

::grpc::Status HEGADBServiceImpl::Execute(
    [[maybe_unused]] ::grpc::ServerContext* context,
    ::hegadb::protocol::ExecuteQuery const* request,
    ::grpc::ServerWriter<::hegadb::protocol::QueryResponse>* writer)
{
    if (request == nullptr || writer == nullptr) {
        return ::grpc::Status(
            ::grpc::StatusCode::INVALID_ARGUMENT,
            "Invalid request or writer");
    }

    if (!streaming_handler_) {
        return ::grpc::Status(
            ::grpc::StatusCode::FAILED_PRECONDITION,
            "No request handler configured");
    }

    try {
        BatchWriter batch_writer = [writer](protocol::QueryResponse const& response) -> bool {
            return writer->Write(response);
        };

        streaming_handler_(*request, batch_writer);

        return ::grpc::Status::OK;
    } catch (std::exception const& ex) {
        return ::grpc::Status(
            ::grpc::StatusCode::INTERNAL,
            ex.what());
    }
}

} // namespace hegadb::transport::grpc
