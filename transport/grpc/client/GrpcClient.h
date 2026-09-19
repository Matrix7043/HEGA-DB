#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#include "query.pb.h"
#include "service.grpc.pb.h"
#include <grpcpp/channel.h>
#include <memory>
namespace hegadb::transport::grpc {

class GrpcClient {

public:
    explicit GrpcClient(std::string const& address);

    protocol::QueryResponse Execute(
        protocol::ExecuteQuery const& request);

private:
    std::shared_ptr<::grpc::Channel> channel_;
    std::unique_ptr<HEGADBService::Stub> stub_;
};

}

#endif // !GRPC_CLIENT_H
