#ifndef HEGADB_SERVICE_IMPL_H
#define HEGADB_SERVICE_IMPL_H

#include "query.pb.h"
#include "service.grpc.pb.h"

#include <functional>
#include <grpcpp/grpcpp.h>

namespace hegadb::transport::grpc {

class HEGADBServiceImpl final : public HEGADBService::Service {
public:
    using RequestHandler = std::function<protocol::QueryResponse(protocol::ExecuteQuery const&)>;

    void setRequestHandler(RequestHandler handler);

    ::grpc::Status Execute(
        ::grpc::ServerContext* context,
        protocol::ExecuteQuery const* request,
        protocol::QueryResponse* response) override;

private:
    RequestHandler request_handler_;
};

}

#endif
