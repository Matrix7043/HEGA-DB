#include "hegadb/transport/grpc/CatalogServiceImpl.h"
#include "catalog.pb.h"
#include "hegadb/transport/types/CatalogTypes.h"

#include <cstddef>
#include <grpcpp/support/status.h>
#include <stdexcept>
#include <utility>

namespace hegadb::transport::grpc {

namespace {

void toProtoColumn(
    transport::CatalogColumn const& source,
    protocol::catalog::ColumnSchema* destination)
{
    destination->set_name(source.name);
    destination->set_data_type(source.data_type);
    destination->set_nullable(source.nullable);
    destination->set_ordinal(source.ordinal);
}

void toProtoTable(
    transport::CatalogTable const& source,
    protocol::catalog::TableSchema* destination)
{
    destination->set_name(source.name);

    for (auto const& column : source.columns) {
        toProtoColumn(
            column,
            destination->add_columns());
    }
}

void fromProtoColumn(
    protocol::catalog::ColumnSchema const& source,
    transport::CatalogColumn* destination)
{
    destination->name = source.name();
    destination->data_type = source.data_type();
    destination->nullable = source.nullable();
    destination->ordinal = source.ordinal();
}

void fromProtoTable(
    protocol::catalog::TableSchema const& source,
    transport::CatalogTable* destination)
{
    destination->name = source.name();

    destination->columns.clear();
    destination->columns.reserve(static_cast<std::size_t>(source.columns_size()));

    for (auto const& column : source.columns()) {
        destination->columns.emplace_back();
        fromProtoColumn(column, &destination->columns.back());
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

void CatalogServiceImpl::setCreateTableHandler(
    CreateTableHandler handler)
{
    create_table_handler_ = std::move(handler);
}

void CatalogServiceImpl::setDropTableHandler(
    DropTableHandler handler)
{
    drop_table_handler_ = std::move(handler);
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

    toProtoTable(
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
        toProtoTable(
            table,
            response->add_tables());
    }

    return ::grpc::Status::OK;
}

::grpc::Status CatalogServiceImpl::CreateTable(
    ::grpc::ServerContext*,
    protocol::catalog::CreateTableRequest const* request,
    protocol::catalog::CreateTableResponse* response)
{
    if (!create_table_handler_) {
        return {
            ::grpc::StatusCode::UNIMPLEMENTED,
            "Catalog handler is not configured"
        };
    }

    try {
        transport::CatalogTable catalog_table;
        fromProtoTable(request->table(), &catalog_table);

        auto version = create_table_handler_(catalog_table);
        response->set_version(version);

    } catch (std::logic_error const&) {
        return {
            ::grpc::StatusCode::ALREADY_EXISTS,
            "Table already exists"
        };
    } catch (std::runtime_error const&) {
        return {
            ::grpc::StatusCode::INTERNAL,
            "Internal error"
        };
    }

    return ::grpc::Status::OK;
}

::grpc::Status CatalogServiceImpl::DropTable(
    ::grpc::ServerContext*,
    protocol::catalog::DropTableRequest const* request,
    protocol::catalog::DropTableResponse* response)
{
    if (!drop_table_handler_) {
        return {
            ::grpc::StatusCode::UNIMPLEMENTED,
            "Catalog handler is not configured"
        };
    }

    try {
        auto version = drop_table_handler_(request->name());
        response->set_version(version);
    } catch (std::logic_error const&) {
        return {
            ::grpc::StatusCode::NOT_FOUND,
            "Table not found"
        };
    } catch (std::runtime_error const&) {
        return {
            ::grpc::StatusCode::INTERNAL,
            "Internal error"
        };
    }

    return ::grpc::Status::OK;
}

} // namespace hegadb::transport::grpc
