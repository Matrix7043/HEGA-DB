#include "hegadb/catalog/CatalogAdapter.h"
#include "hegadb/catalog/service/CatalogService.h"

#include "hegadb/transport/CatalogClientTransportFactory.h"
#include "hegadb/transport/CatalogServerTransport.h"
#include "hegadb/transport/CatalogServerTransportFactory.h"
#include "hegadb/transport/grpc/CatalogServiceImpl.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    using namespace hegadb;

    constexpr char SERVER_ADDRESS[] = "127.0.0.1:50083";

    // --------------------------------------------------------
    // 1. Create the catalog domain service.
    // --------------------------------------------------------

    catalog::service::CatalogService catalog_service;

    // --------------------------------------------------------
    // 2. Configure the gRPC service handlers.
    // --------------------------------------------------------

    auto grpc_service = std::make_shared<transport::grpc::CatalogServiceImpl>();

    grpc_service->setGetCatalogVersionHandler(
        [&catalog_service] {
            return catalog_service.getCatalogVersion();
        });

    grpc_service->setGetTableHandler(
        [&catalog_service](std::string const& table_name)
            -> std::optional<transport::CatalogTable> {
            auto const* table = catalog_service.getTable(table_name);

            if (table == nullptr) {
                return std::nullopt;
            }

            return catalog::CatalogAdapter::toTransport(*table);
        });

    grpc_service->setGetCatalogHandler(
        [&catalog_service] {
            return catalog::CatalogAdapter::toTransport(
                catalog_service.getCatalog());
        });

    // --------------------------------------------------------
    // 3. Create and start the server through its factory.
    // --------------------------------------------------------

    auto server = transport::CatalogServerTransportFactory::createGrpc(
        SERVER_ADDRESS,
        grpc_service);

    server->start();

    try {
        // ----------------------------------------------------
        // 4. Create the client through its factory.
        // ----------------------------------------------------

        auto client = transport::CatalogClientTransportFactory::createGrpc(
            SERVER_ADDRESS);

        // ----------------------------------------------------
        // Test 1: Retrieve catalog version.
        // ----------------------------------------------------

        auto version = client->getCatalogVersion();

        assert(version == 1);

        std::cout << "[PASS] Get catalog version\n";

        // ----------------------------------------------------
        // Test 2: Retrieve the existing users table.
        // ----------------------------------------------------

        auto users = client->getTable("users");

        assert(users.has_value());
        assert(users->name == "users");
        assert(users->columns.size() == 3);

        assert(users->columns[0].name == "id");
        assert(users->columns[1].name == "name");
        assert(users->columns[2].name == "active");

        std::cout << "[PASS] Get users table\n";

        // ----------------------------------------------------
        // Test 3: Retrieve the existing orders table.
        // ----------------------------------------------------

        auto orders = client->getTable("orders");

        assert(orders.has_value());
        assert(orders->name == "orders");
        assert(orders->columns.size() == 3);

        std::cout << "[PASS] Get orders table\n";

        // ----------------------------------------------------
        // Test 4: Request a nonexistent table.
        // ----------------------------------------------------

        auto missing = client->getTable("does_not_exist");

        assert(!missing.has_value());

        std::cout << "[PASS] Missing table handling\n";

        // ----------------------------------------------------
        // Test 5: Retrieve the complete catalog.
        // ----------------------------------------------------

        auto description = client->getCatalog();

        assert(description.version == 1);
        assert(description.tables.size() == 2);

        bool found_users = false;
        bool found_orders = false;

        for (auto const& table : description.tables) {
            if (table.name == "users") {
                found_users = true;
            }

            if (table.name == "orders") {
                found_orders = true;
            }
        }

        assert(found_users);
        assert(found_orders);

        std::cout << "[PASS] Get complete catalog\n";

        // ----------------------------------------------------
        // 6. Shut down the server.
        // ----------------------------------------------------

        server->shutdown();

        std::cout << "\nAll Catalog Service integration tests passed.\n";
    } catch (std::exception const& ex) {
        server->shutdown();

        std::cerr << "[FAIL] Catalog integration test: "
                  << ex.what() << '\n';

        return 1;
    }

    return 0;
}
