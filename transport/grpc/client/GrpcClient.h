#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#include "hegadb/transport/ClientTransport.h"
#include "query.pb.h"
#include "service.grpc.pb.h"

#include <memory>

namespace hegadb::transport::grpc {

class GrpcClient final : public hegadb::transport::ClientTransport {

public:
    explicit GrpcClient(std::string const& address);

    protocol::QueryResponse execute(
        protocol::ExecuteQuery const& request) override;

private:
    std::unique_ptr<HEGADBService::Stub> stub_;
};

}

#endif // !GRPC_CLIENT_H
