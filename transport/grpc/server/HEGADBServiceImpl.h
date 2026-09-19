#ifndef HEGADB_SERVICE_IMPL_H
#define HEGADB_SERVICE_IMPL_H

#include "query.pb.h"
#include "service.grpc.pb.h"

#include <grpcpp/grpcpp.h>

namespace hegadb::transport::grpc {

class HEGADBServiceImpl final : public HEGADBService::Service {
public:
    ::grpc::Status Execute(
        ::grpc::ServerContext* context,
        protocol::ExecuteQuery const* request,
        protocol::QueryResponse* response) override;
};

}

#endif
