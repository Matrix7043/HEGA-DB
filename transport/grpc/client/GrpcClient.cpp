#include "GrpcClient.h"
#include "query.pb.h"
#include "service.grpc.pb.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/status.h>
#include <stdexcept>
#include <string>

namespace hegadb::transport::grpc {

GrpcClient::GrpcClient(std::string const& address)
    : stub_(HEGADBService::NewStub(
          ::grpc::CreateChannel(address,
              ::grpc::InsecureChannelCredentials())))
{
}

protocol::QueryResponse GrpcClient::execute(
    protocol::ExecuteQuery const& request)
{
    protocol::QueryResponse response;

    ::grpc::ClientContext context;

    ::grpc::Status const status = stub_->Execute(&context, request, &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "gRPC Execute failed: " + status.error_message());
    }

    return response;
}

}
