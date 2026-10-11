#include "hegadb/catalog/CatalogAdapter.h"
#include "hegadb/catalog/service/CatalogService.h"

#include "hegadb/transport/CatalogServerTransport.h"
#include "hegadb/transport/CatalogServerTransportFactory.h"
#include "hegadb/transport/grpc/CatalogServiceImpl.h"

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    try {
        hegadb::catalog::service::CatalogService catalog_service;

        auto grpc_service = std::make_shared<
            hegadb::transport::grpc::CatalogServiceImpl>();

        grpc_service->setGetCatalogVersionHandler(
            [&catalog_service] {
                return catalog_service.getCatalogVersion();
            });

        grpc_service->setGetTableHandler(
            [&catalog_service](
                std::string const& table_name)
                -> std::optional<
                    hegadb::transport::CatalogTable> {
                auto const* table = catalog_service.getTable(table_name);

                if (table == nullptr) {
                    return std::nullopt;
                }

                return hegadb::catalog::CatalogAdapter::
                    toTransport(*table);
            });

        grpc_service->setGetCatalogHandler(
            [&catalog_service] {
                return hegadb::catalog::CatalogAdapter::
                    toTransport(catalog_service.getCatalog());
            });

        auto server = hegadb::transport::CatalogServerTransportFactory::createGrpc(
            "0.0.0.0:50052",
            grpc_service);

        server->start();

        std::cout << "Catalog Service started on "
                  << "0.0.0.0:50052\n";

        server->wait();

        std::cout << "Catalog Service stopped.\n";

    } catch (std::exception const& ex) {
        std::cerr << "Catalog Service error: "
                  << ex.what() << '\n';
        return 1;
    }

    return 0;
}
