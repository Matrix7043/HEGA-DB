#include "hegadb/transport/grpc/GrpcCatalogMutationClient.h"
#include "catalog.pb.h"
#include "catalog_service.grpc.pb.h"
#include "hegadb/transport/types/CatalogTypes.h"
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <stdexcept>

namespace hegadb::transport::grpc {

namespace {

protocol::catalog::ColumnSchema toProtoColumn(
    transport::CatalogColumn const& column)
{
    protocol::catalog::ColumnSchema result;

    result.set_name(column.name);
    result.set_data_type(column.data_type);
    result.set_nullable(column.nullable);
    result.set_ordinal(column.ordinal);

    return result;
}

protocol::catalog::TableSchema toProtoTable(
    transport::CatalogTable const& table)
{
    protocol::catalog::TableSchema result;

    result.set_name(table.name);

    for (auto const& column : table.columns) {
        *result.add_columns() = toProtoColumn(column);
    }

    return result;
}

} // namespace

GrpcCatalogMutationClient::GrpcCatalogMutationClient(
    std::string const& address)
    : stub_(CatalogService::NewStub(
          ::grpc::CreateChannel(address,
              ::grpc::InsecureChannelCredentials())))
{
}

transport::CatalogVersion
GrpcCatalogMutationClient::getCatalogVersion()
{
    protocol::catalog::GetCatalogVersionRequest request;
    protocol::catalog::GetCatalogVersionResponse response;

    ::grpc::ClientContext context;

    auto status = stub_->GetCatalogVersion(&context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "GetCatalogVersion RPC failed: " + status.error_message());
    }

    return response.version();
}

void GrpcCatalogMutationClient::createTable(CatalogTable table)
{
    protocol::catalog::CreateTableRequest request;
    protocol::catalog::CreateTableResponse response;

    *request.mutable_table() = toProtoTable(table);

    ::grpc::ClientContext context;

    auto status = stub_->CreateTable(&context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "CreateTable RPC failed: " + status.error_message());
    }
}

void GrpcCatalogMutationClient::dropTable(std::string const& table_name)
{
    protocol::catalog::DropTableRequest request;
    protocol::catalog::DropTableResponse response;

    request.set_name(table_name);

    ::grpc::ClientContext context;

    auto status = stub_->DropTable(&context,
        request,
        &response);

    if (!status.ok()) {
        throw std::runtime_error(
            "DropTable RPC failed: " + status.error_message());
    }
}

}
