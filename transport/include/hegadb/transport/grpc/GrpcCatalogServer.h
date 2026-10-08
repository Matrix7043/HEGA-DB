#ifndef GRPC_CATALOG_SERVER_H
#define GRPC_CATALOG_SERVER_H

#include "CatalogServiceImpl.h"
#include <hegadb/transport/CatalogServerTransport.h>
#include <memory>

namespace hegadb::transport::grpc {

class GrpcCatalogServer final : public CatalogServerTransport {

public:
    GrpcCatalogServer(
        std::string address,
        std::shared_ptr<CatalogServiceImpl> service);

    ~GrpcCatalogServer() override;

    void start() override;
    void shutdown() override;

private:
    std::string address_;
    std::shared_ptr<CatalogServiceImpl> service_;
    std::unique_ptr<::grpc::Server> server_;
};

}

#endif
