#ifndef GRPC_CATALOG_MUTATION_CLIENT
#define GRPC_CATALOG_MUTATION_CLIENT

#include "catalog_service.grpc.pb.h"
#include "hegadb/transport/CatalogMutationClientTransport.h"
#include "hegadb/transport/types/CatalogTypes.h"
#include <memory>

namespace hegadb::transport::grpc {

class GrpcCatalogMutationClient final : public transport::CatalogMutationClientTransport {

public:
    explicit GrpcCatalogMutationClient(std::string const& address);

    CatalogVersion getCatalogVersion() override;

    void createTable(CatalogTable table) override;

    void dropTable(std::string const& table_name) override;

private:
    std::unique_ptr<CatalogService::Stub> stub_;
};

}

#endif
