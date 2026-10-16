#include "hegadb/transport/grpc/GrpcCatalogClient.h"
#include "hegadb/transport/types/CatalogTypes.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <stdexcept>

namespace hegadb::transport::grpc {

namespace {

transport::CatalogColumn toCatalogColumn(
    protocol::catalog::ColumnSchema const& column)
{
    return transport::CatalogColumn {
        .name = column.name(),
        .data_type = column.data_type(),
        .nullable = column.nullable(),
        .ordinal = column.ordinal()
    };
}

transport::CatalogTable toCatalogTable(
    protocol::catalog::TableSchema const& table)
{
    transport::CatalogTable result;

    result.name = table.name();

    result.columns.reserve(
        static_cast<std::size_t>(table.columns_size()));

    for (auto const& column : table.columns()) {
        result.columns.push_back(toCatalogColumn(column));
    }

    return result;
}

} // namespace

GrpcCatalogClient::GrpcCatalogClient(
    std::string const& address)
    : stub_(CatalogService::NewStub(
          ::grpc::CreateChannel(address, ::grpc::InsecureChannelCredentials())))
{
    if (!stub_) {
        throw std::invalid_argument(
            "GrpcCatalogClient requires a valid gRPC channel");
    }
}

transport::CatalogVersion
GrpcCatalogClient::getCatalogVersion()
{
    protocol::catalog::GetCatalogVersionRequest request;
    protocol::catalog::GetCatalogVersionResponse response;

    ::grpc::ClientContext context;

    auto status = stub_->GetCatalogVersion(
        &context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "GetCatalogVersion RPC failed: " + status.error_message());
    }

    return response.version();
}

std::optional<transport::CatalogTable>
GrpcCatalogClient::getTable(
    std::string const& table_name)
{
    protocol::catalog::GetTableRequest request;
    request.set_table_name(table_name);

    protocol::catalog::GetTableResponse response;

    ::grpc::ClientContext context;

    auto status = stub_->GetTable(
        &context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "GetTable RPC failed: " + status.error_message());
    }

    if (!response.found()) {
        return std::nullopt;
    }

    return toCatalogTable(response.table());
}

transport::CatalogDescription
GrpcCatalogClient::getCatalog()
{
    protocol::catalog::GetCatalogRequest request;
    protocol::catalog::GetCatalogResponse response;

    ::grpc::ClientContext context;

    auto status = stub_->GetCatalog(
        &context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "GetCatalog RPC failed: " + status.error_message());
    }

    transport::CatalogDescription snapshotDescription;

    snapshotDescription.version = response.version();

    snapshotDescription.tables.reserve(
        static_cast<std::size_t>(response.tables_size()));

    for (auto const& table : response.tables()) {
        snapshotDescription.tables.push_back(
            toCatalogTable(table));
    }

    return snapshotDescription;
}

} // namespace hegadb::transport::grpc
