#ifndef HEGADB_CATALOG_SERVICE_IMPL_H
#define HEGADB_CATALOG_SERVICE_IMPL_H

#include "hegadb/transport/types/CatalogTypes.h"

#include "catalog_service.grpc.pb.h"

#include <functional>
#include <grpcpp/grpcpp.h>
#include <optional>
#include <string>

namespace hegadb::transport::grpc {

class CatalogServiceImpl final
    : public CatalogService::Service {
public:
    using GetCatalogVersionHandler = std::function<transport::CatalogVersion()>;

    using GetTableHandler = std::function<std::optional<transport::CatalogTable>(
        std::string const& table_name)>;

    using GetCatalogHandler = std::function<transport::CatalogDescription()>;

    void setGetCatalogVersionHandler(
        GetCatalogVersionHandler handler);

    void setGetTableHandler(
        GetTableHandler handler);

    void setGetCatalogHandler(
        GetCatalogHandler handler);

    ::grpc::Status GetCatalogVersion(
        ::grpc::ServerContext* context,
        protocol::catalog::GetCatalogVersionRequest const* request,
        protocol::catalog::GetCatalogVersionResponse* response) override;

    ::grpc::Status GetTable(
        ::grpc::ServerContext* context,
        protocol::catalog::GetTableRequest const* request,
        protocol::catalog::GetTableResponse* response) override;

    ::grpc::Status GetCatalog(
        ::grpc::ServerContext* context,
        protocol::catalog::GetCatalogRequest const* request,
        protocol::catalog::GetCatalogResponse* response) override;

private:
    GetCatalogVersionHandler get_catalog_version_handler_;
    GetTableHandler get_table_handler_;
    GetCatalogHandler get_catalog_handler_;
};

} // namespace hegadb::transport::grpc

#endif
