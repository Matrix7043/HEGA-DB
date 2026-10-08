#ifndef HEGADB_GRPC_CATALOG_CLIENT_H
#define HEGADB_GRPC_CATALOG_CLIENT_H

#include "catalog_service.grpc.pb.h"
#include "hegadb/transport/CatalogClientTransport.h"
#include "hegadb/transport/types/CatalogTypes.h"
#include <grpcpp/grpcpp.h>
#include <memory>
#include <optional>

namespace hegadb::transport::grpc {

class GrpcCatalogClient final : public transport::CatalogClientTransport {
public:
    explicit GrpcCatalogClient(std::shared_ptr<::grpc::Channel> channel);

    transport::CatalogVersion getCatalogVersion() override;

    std::optional<transport::CatalogTable> getTable(
        std::string const& table_name) override;

    transport::CatalogDescription getCatalog() override;

private:
    std::unique_ptr<CatalogService::Stub> stub_;
};

}

#endif // !HEGADB_GRPC_CATALOG_CLIENT_H
