#include "hegadb/transport/grpc/CatalogServiceImpl.h"

#include <utility>

namespace hegadb::transport::grpc {

namespace {

void fillProtoColumn(
    transport::CatalogColumn const& source,
    protocol::catalog::ColumnSchema* destination)
{
    destination->set_name(source.name);
    destination->set_data_type(source.data_type);
    destination->set_nullable(source.nullable);
    destination->set_ordinal(source.ordinal);
}

void fillProtoTable(
    transport::CatalogTable const& source,
    protocol::catalog::TableSchema* destination)
{
    destination->set_name(source.name);

    for (auto const& column : source.columns) {
        fillProtoColumn(
            column,
            destination->add_columns());
    }
}

} // namespace

void CatalogServiceImpl::setGetCatalogVersionHandler(
    GetCatalogVersionHandler handler)
{
    get_catalog_version_handler_ = std::move(handler);
}

void CatalogServiceImpl::setGetTableHandler(
    GetTableHandler handler)
{
    get_table_handler_ = std::move(handler);
}

void CatalogServiceImpl::setGetCatalogHandler(
    GetCatalogHandler handler)
{
    get_catalog_handler_ = std::move(handler);
}

::grpc::Status CatalogServiceImpl::GetCatalogVersion(
    ::grpc::ServerContext*,
    protocol::catalog::GetCatalogVersionRequest const*,
    protocol::catalog::GetCatalogVersionResponse* response)
{
    if (!get_catalog_version_handler_) {
        return {
            ::grpc::StatusCode::UNIMPLEMENTED,
            "Catalog version handler is not configured"
        };
    }

    response->set_version(
        get_catalog_version_handler_());

    return ::grpc::Status::OK;
}

::grpc::Status CatalogServiceImpl::GetTable(
    ::grpc::ServerContext*,
    protocol::catalog::GetTableRequest const* request,
    protocol::catalog::GetTableResponse* response)
{
    if (!get_table_handler_) {
        return {
            ::grpc::StatusCode::UNIMPLEMENTED,
            "Catalog table handler is not configured"
        };
    }

    auto table = get_table_handler_(
        request->table_name());

    if (!table.has_value()) {
        response->set_found(false);
        return ::grpc::Status::OK;
    }

    response->set_found(true);

    fillProtoTable(
        table.value(),
        response->mutable_table());

    return ::grpc::Status::OK;
}

::grpc::Status CatalogServiceImpl::GetCatalog(
    ::grpc::ServerContext*,
    protocol::catalog::GetCatalogRequest const*,
    protocol::catalog::GetCatalogResponse* response)
{
    if (!get_catalog_handler_) {
        return {
            ::grpc::StatusCode::UNIMPLEMENTED,
            "Catalog handler is not configured"
        };
    }

    auto catalog = get_catalog_handler_();

    response->set_version(catalog.version);

    for (auto const& table : catalog.tables) {
        fillProtoTable(
            table,
            response->add_tables());
    }

    return ::grpc::Status::OK;
}

} // namespace hegadb::transport::grpc
