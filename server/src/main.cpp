#include <hegadb/transport/ServerTransportFactory.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/transport/ServerTransport.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <unordered_map>

int main(int argc, char** argv)
{
    std::string server_address = "0.0.0.0:50051";

    std::cout << "Starting HEGA-DB Server on " << server_address << "...\n";

    // Initialize catalog with empty snapshot
    std::unordered_map<std::string, hegadb::catalog::TableSchema> tables;
    auto snapshot = std::make_shared<hegadb::catalog::CatalogSnapshot const>(1, std::move(tables));
    auto catalog = std::make_shared<hegadb::catalog::InMemoryCatalog>(snapshot);
    
    // Initialize crypto context
    auto crypto_context = std::make_shared<hegadb::crypto::OpenFHEContext>();
    crypto_context->enableFeatures();
    
    // Initialize storage manager with file-based storage
    // Default to "./data" directory - can be configured via command line args
    std::string data_dir = (argc > 1) ? argv[1] : "./data";
    
    std::shared_ptr<hegadb::server::storage::StorageManager> storage;
    try {
        storage = hegadb::server::storage::StorageManager::createFileStorage(data_dir);
        std::cout << "Storage initialized from directory: " << data_dir << "\n";
    } catch (std::exception const& e) {
        std::cerr << "Warning: Could not initialize file storage: " << e.what() << "\n";
        std::cerr << "Creating mock storage for testing...\n";
        
        // Fallback to mock storage for testing
        class MockStorageManager : public hegadb::server::storage::StorageManager {
        public:
            std::unique_ptr<hegadb::server::storage::EncryptedTable> openTable(
                std::string const& table_name) override {
                throw std::runtime_error("Mock storage: table not found: " + table_name);
            }
        };
        storage = std::make_shared<MockStorageManager>();
    }
    
    // Create query executor
    auto executor = std::make_shared<hegadb::server::QueryExecutor>(
        catalog,
        storage,
        crypto_context
    );

    // Instantiate Query Receiver
    auto receiver = std::make_shared<hegadb::server::ExecuteQueryReceiver>(
        catalog,
        executor
    );

    // Instantiate gRPC Transport Server
    auto server = hegadb::transport::ServerTransportFactory::createGrpc(server_address);

    server->setRequestHandler(
        [receiver](hegadb::protocol::ExecuteQuery const& request) {
            if (request.has_request_id())
                std::cout << request.request_id().SerializeAsString() << std::endl;
            return receiver->executeQuery(request);
        });

    server->start();
    std::cout << "HEGA-DB Server ready to receive ExecuteQuery requests.\n";

    server->wait();

    return 0;
}
