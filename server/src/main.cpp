#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/execution/CatalogLoader.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/logging/LoggingConfig.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/transport/ServerTransport.h>
#include <hegadb/transport/ServerTransportFactory.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <unordered_map>

int main(int argc, char** argv)
{
    hegadb::logging::LoggingConfig logging_config;
    logging_config.console_enabled = true;
    logging_config.level = spdlog::level::info;
    hegadb::logging::initialize(logging_config);

    std::string server_address = "0.0.0.0:50051";

    hegadb::logging::info("Starting HEGA-DB Server on {}...", server_address);

    std::string home_dir = std::getenv("HOME") ? std::getenv("HOME") : ".";
    std::string data_dir    = (argc > 1) ? argv[1] : (home_dir + "/.hegadb/data");
    std::string catalog_dir = (argc > 2) ? argv[2] : (home_dir + "/.hegadb/catalog");

    // ----------------------------------------------------------------
    // Catalog — load schema from on-disk manifests written by
    // hegadb_setup.  Falls back to an empty catalog if the directory
    // does not yet exist so the server still starts cleanly.
    // ----------------------------------------------------------------
    std::shared_ptr<hegadb::catalog::InMemoryCatalog> catalog;
    try {
        catalog = hegadb::execution::CatalogLoader::loadFromDirectory(catalog_dir);
        hegadb::logging::info("Catalog loaded from: {}", catalog_dir);
    } catch (std::exception const& e) {
        hegadb::logging::warn(
            "Could not load catalog from '{}': {} — starting with empty catalog",
            catalog_dir, e.what());

        std::unordered_map<std::string, hegadb::catalog::TableSchema> tables;
        auto snapshot = std::make_shared<hegadb::catalog::CatalogSnapshot const>(
            1, std::move(tables));
        catalog = std::make_shared<hegadb::catalog::InMemoryCatalog>(snapshot);
    }

    // ----------------------------------------------------------------
    // Crypto context
    // ----------------------------------------------------------------
    auto crypto_context = std::make_shared<hegadb::crypto::OpenFHEContext>();
    crypto_context->enableFeatures();

    // ----------------------------------------------------------------
    // Storage manager — file-based, reading encrypted segment files
    // written by hegadb_setup.  Falls back to a no-op stub on error.
    // ----------------------------------------------------------------
    std::shared_ptr<hegadb::server::storage::StorageManager> storage;
    try {
        storage = hegadb::server::storage::StorageManager::createFileStorage(
            data_dir, catalog_dir);
        hegadb::logging::info(
            "Storage initialized: data={} catalog={}", data_dir, catalog_dir);
    } catch (std::exception const& e) {
        hegadb::logging::warn(
            "Could not initialize file storage: {} — falling back to empty storage",
            e.what());

        class EmptyStorageManager : public hegadb::server::storage::StorageManager {

            class EmptyTable : public hegadb::server::storage::EncryptedTable {
                std::string name_;
            public:
                explicit EmptyTable(std::string n) : name_(std::move(n)) {}
                std::string const& getName() const override { return name_; }
                std::vector<std::string> getColumnNames() const override { return {}; }
                void open() override {}
                std::optional<hegadb::server::storage::EncryptedBatch>
                nextBatch() override { return std::nullopt; }
                void close() override {}
                size_t getRowCount() const override { return 0; }
            };

        public:
            std::unique_ptr<hegadb::server::storage::EncryptedTable>
            openTable(std::string const& table_name) override {
                return std::make_unique<EmptyTable>(table_name);
            }
        };

        storage = std::make_shared<EmptyStorageManager>();
    }

    // ----------------------------------------------------------------
    // Query executor and receiver
    // ----------------------------------------------------------------
    auto executor = std::make_shared<hegadb::server::QueryExecutor>(
        catalog, storage, crypto_context);

    auto receiver = std::make_shared<hegadb::server::ExecuteQueryReceiver>(
        catalog, executor);

    // ----------------------------------------------------------------
    // gRPC transport
    // ----------------------------------------------------------------
    auto server = hegadb::transport::ServerTransportFactory::createGrpc(server_address);

    server->setStreamingRequestHandler(
        [receiver](hegadb::protocol::ExecuteQuery const& request,
                   hegadb::transport::ServerTransport::BatchWriter writer) {
            if (request.has_request_id())
                hegadb::logging::debug(
                    "Request ID: {}", request.request_id().value());
            receiver->executeQueryStream(request, std::move(writer));
        });

    server->start();
    hegadb::logging::info("HEGA-DB Server ready.");

    server->wait();

    return 0;
}
