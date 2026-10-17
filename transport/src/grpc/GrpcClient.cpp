#include "hegadb/transport/grpc/GrpcClient.h"
#include "query.pb.h"
#include "service.grpc.pb.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/status.h>
#include <stdexcept>
#include <string>

namespace hegadb::transport::grpc {

namespace {
    /*
     * A single serialized BFV ciphertext is ~1 MiB; a full QueryResponse can
     * carry several columns × several segments, so the default 4 MiB receive
     * limit is easily exceeded.  Match the server's 64 MiB send limit.
     */
    constexpr int MAX_MESSAGE_BYTES = 64 * 1024 * 1024;
} // namespace

GrpcClient::GrpcClient(std::string const& address)
{
    ::grpc::ChannelArguments args;
    args.SetMaxReceiveMessageSize(MAX_MESSAGE_BYTES);
    args.SetMaxSendMessageSize(MAX_MESSAGE_BYTES);

    stub_ = HEGADBService::NewStub(
        ::grpc::CreateCustomChannel(
            address,
            ::grpc::InsecureChannelCredentials(),
            args));
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
