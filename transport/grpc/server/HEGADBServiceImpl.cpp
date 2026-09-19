#include "HEGADBServiceImpl.h"
#include "common.pb.h"
#include <grpcpp/support/status.h>

namespace hegadb::transport::grpc {

::grpc::Status HEGADBServiceImpl::Execute(
    ::grpc::ServerContext* context,
    protocol::ExecuteQuery const* request,
    protocol::QueryResponse* response)
{

    (void)context;

    // execution will be implemented later

    if (request->has_protocol_version()) {
        response->mutable_protocol_version()
            ->CopyFrom(request->protocol_version());
    }

    if (request->has_request_id()) {
        response->mutable_request_id()
            ->CopyFrom(request->request_id());
    }

    response->mutable_status()->set_code(protocol::STATUS_OK);

    response->mutable_status()->set_message("gRPC transport request received");

    return ::grpc::Status::OK;
}

}
