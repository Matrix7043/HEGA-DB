#include "hegadb/transport/grpc/CatalogServiceImpl.h"
#include "hegadb/transport/grpc/GrpcCatalogClient.h"
#include "hegadb/transport/grpc/GrpcCatalogServer.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace {

using hegadb::transport::CatalogColumn;
using hegadb::transport::CatalogDescription;
using hegadb::transport::CatalogTable;
using hegadb::transport::grpc::CatalogServiceImpl;
using hegadb::transport::grpc::GrpcCatalogClient;
using hegadb::transport::grpc::GrpcCatalogServer;

CatalogDescription createCatalog()
{
    CatalogDescription catalog;

    catalog.version = 42;

    catalog.tables = {
        CatalogTable {
            .name = "users",
            .columns = {
                CatalogColumn {
                    .name = "id",
                    .data_type = "INT",
                    .nullable = false,
                    .ordinal = 0 },
                CatalogColumn {
                    .name = "name",
                    .data_type = "TEXT",
                    .nullable = false,
                    .ordinal = 1 },
                CatalogColumn {
                    .name = "email",
                    .data_type = "TEXT",
                    .nullable = true,
                    .ordinal = 2 } } },
        CatalogTable { .name = "orders", .columns = { CatalogColumn { .name = "id", .data_type = "INT", .nullable = false, .ordinal = 0 }, CatalogColumn { .name = "user_id", .data_type = "INT", .nullable = false, .ordinal = 1 } } }
    };

    return catalog;
}

void testGetCatalogVersion(
    GrpcCatalogClient& client)
{

    auto version = client.getCatalogVersion();

    assert(version == 42);

    std::cout << "[PASS] GetCatalogVersion\n";
}

void testGetExistingTable(
    GrpcCatalogClient& client)
{

    auto table = client.getTable("users");

    assert(table.has_value());
    assert(table->name == "users");

    assert(table->columns.size() == 3);

    assert(table->columns[0].name == "id");
    assert(table->columns[0].data_type == "INT");
    assert(!table->columns[0].nullable);
    assert(table->columns[0].ordinal == 0);

    assert(table->columns[1].name == "name");
    assert(table->columns[1].data_type == "TEXT");
    assert(!table->columns[1].nullable);
    assert(table->columns[1].ordinal == 1);

    assert(table->columns[2].name == "email");
    assert(table->columns[2].data_type == "TEXT");
    assert(table->columns[2].nullable);
    assert(table->columns[2].ordinal == 2);

    std::cout << "[PASS] GetTable(existing)\n";
}

void testGetMissingTable(
    GrpcCatalogClient& client)
{

    auto table = client.getTable("does_not_exist");

    assert(!table.has_value());

    std::cout << "[PASS] GetTable(missing)\n";
}

void testGetCatalog(
    GrpcCatalogClient& client)
{

    auto catalog = client.getCatalog();

    assert(catalog.version == 42);
    assert(catalog.tables.size() == 2);

    auto const& users = catalog.tables[0];

    assert(users.name == "users");
    assert(users.columns.size() == 3);

    auto const& orders = catalog.tables[1];

    assert(orders.name == "orders");
    assert(orders.columns.size() == 2);

    std::cout << "[PASS] GetCatalog\n";
}

} // namespace

int main()
{

    auto catalog = createCatalog();

    auto service = std::make_shared<CatalogServiceImpl>();

    service->setGetCatalogVersionHandler(
        [catalog]() {
            return catalog.version;
        });

    service->setGetTableHandler(
        [catalog](std::string const& table_name)
            -> std::optional<CatalogTable> {
            for (auto const& table : catalog.tables) {
                if (table.name == table_name) {
                    return table;
                }
            }

            return std::nullopt;
        });

    service->setGetCatalogHandler(
        [catalog]() {
            return catalog;
        });

    constexpr char const* address = "127.0.0.1:50052";

    GrpcCatalogServer server(
        address,
        service);

    server.start();

    auto channel = grpc::CreateChannel(
        address,
        grpc::InsecureChannelCredentials());

    GrpcCatalogClient client(channel);

    // Give the server a moment to become ready.
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));

    testGetCatalogVersion(client);
    testGetExistingTable(client);
    testGetMissingTable(client);
    testGetCatalog(client);

    server.shutdown();

    std::cout
        << "\nAll catalog transport tests passed.\n";

    return 0;
}
