#include "hegadb/catalog/CatalogAdapter.h"
#include "hegadb/catalog/service/CatalogService.h"

#include "hegadb/transport/CatalogClientTransportFactory.h"
#include "hegadb/transport/CatalogMutationClientTransportFactory.h"
#include "hegadb/transport/CatalogServerTransport.h"
#include "hegadb/transport/CatalogServerTransportFactory.h"
#include "hegadb/transport/grpc/CatalogServiceImpl.h"
#include "hegadb/transport/types/CatalogTypes.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
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

    grpc_service->setCreateTableHandler(
        [&catalog_service](transport::CatalogTable table)
            -> transport::CatalogVersion {
            return catalog_service.createTable(
                catalog::CatalogAdapter::fromTransport(table));
        });

    grpc_service->setDropTableHandler(
        [&catalog_service](std::string const& table_name)
            -> transport::CatalogVersion {
            return catalog_service.dropTable(table_name);
        });

    // --------------------------------------------------------
    // 3. Start the server.
    // --------------------------------------------------------

    auto server = transport::CatalogServerTransportFactory::createGrpc(
        SERVER_ADDRESS,
        grpc_service);

    server->start();

    try {
        // ----------------------------------------------------
        // 4. Create read and mutation clients.
        // ----------------------------------------------------

        auto client = transport::CatalogClientTransportFactory::createGrpc(
            SERVER_ADDRESS);

        auto mutation_client = transport::CatalogMutationClientTransportFactory::createGrpc(
            SERVER_ADDRESS);

        // ----------------------------------------------------
        // Test 1: Retrieve catalog version.
        // ----------------------------------------------------

        assert(client->getCatalogVersion() == 1);

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

        std::cout << "[PASS] Missing table lookup\n";

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
        // Test 6: Create a table through gRPC.
        // ----------------------------------------------------

        transport::CatalogTable new_table;
        new_table.name = "integration_mutation_test";

        transport::CatalogColumn id_column;
        id_column.name = "id";
        id_column.data_type = "INTEGER";
        id_column.nullable = false;
        id_column.ordinal = 0;

        transport::CatalogColumn label_column;
        label_column.name = "label";
        label_column.data_type = "STRING";
        label_column.nullable = false;
        label_column.ordinal = 1;

        new_table.columns.push_back(id_column);
        new_table.columns.push_back(label_column);

        mutation_client->createTable(new_table);

        // Verify through the independent read client.
        assert(client->getCatalogVersion() == 2);

        auto created = client->getTable("integration_mutation_test");

        assert(created.has_value());
        assert(created->name == "integration_mutation_test");
        assert(created->columns.size() == 2);
        assert(created->columns[0].name == "id");
        assert(created->columns[1].name == "label");

        std::cout << "[PASS] Create table through gRPC\n";

        // ----------------------------------------------------
        // Test 7: Duplicate table creation must fail.
        // ----------------------------------------------------

        bool duplicate_create_failed = false;

        try {
            mutation_client->createTable(new_table);
        } catch (std::runtime_error const&) {
            duplicate_create_failed = true;
        }

        assert(duplicate_create_failed);

        // Failed mutation must not increment the version.
        assert(client->getCatalogVersion() == 2);
        assert(client->getTable("integration_mutation_test").has_value());

        std::cout << "[PASS] Duplicate table creation rejected\n";

        // ----------------------------------------------------
        // Test 8: Drop the created table through gRPC.
        // ----------------------------------------------------

        mutation_client->dropTable("integration_mutation_test");

        assert(client->getCatalogVersion() == 3);
        assert(!client->getTable("integration_mutation_test").has_value());

        // Original tables must still exist.
        assert(client->getTable("users").has_value());
        assert(client->getTable("orders").has_value());

        std::cout << "[PASS] Drop table through gRPC\n";

        // ----------------------------------------------------
        // Test 9: Dropping a nonexistent table must fail.
        // ----------------------------------------------------

        bool missing_drop_failed = false;

        try {
            mutation_client->dropTable("integration_mutation_test");
        } catch (std::runtime_error const&) {
            missing_drop_failed = true;
        }

        assert(missing_drop_failed);

        // Failed deletion must not increment the version.
        assert(client->getCatalogVersion() == 3);
        assert(!client->getTable("integration_mutation_test").has_value());

        std::cout << "[PASS] Deletion of missing table rejected\n";

        // ----------------------------------------------------
        // Shut down the server.
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
