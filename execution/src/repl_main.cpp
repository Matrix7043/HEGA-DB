#include <hegadb/execution/ExecutionREPL.h>
#include <hegadb/execution/LocalExecutor.h>
#include <hegadb/execution/CatalogLoader.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/logging/LoggingConfig.h>

#include <iostream>
#include <memory>

/**
 * @brief Main entry point for HEGA-DB local execution REPL
 * 
 * This executable provides a standalone query execution environment
 * for testing and development. It integrates client-side and server-side
 * components without requiring network communication.
 */
int main(int argc, char** argv) {
    try {
        // Parse command-line arguments
        std::string home_dir    = std::getenv("HOME") ? std::getenv("HOME") : ".";
        std::string data_dir    = home_dir + "/.hegadb/data";
        std::string catalog_dir = home_dir + "/.hegadb/catalog";
        std::string log_level   = "info";

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--data-dir" && i + 1 < argc) {
                data_dir = argv[++i];
            } else if (arg == "--catalog-dir" && i + 1 < argc) {
                catalog_dir = argv[++i];
            } else if (arg == "--log-level" && i + 1 < argc) {
                log_level = argv[++i];
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: " << argv[0] << " [OPTIONS]\n\n";
                std::cout << "Options:\n";
                std::cout << "  --data-dir <path>      Data directory (default: ~/.hegadb/data)\n";
                std::cout << "  --catalog-dir <path>   Catalog directory (default: ~/.hegadb/catalog)\n";
                std::cout << "  --log-level <level>    Log level: debug|info|warn|error (default: info)\n";
                std::cout << "  --help, -h             Show this help message\n";
                std::cout << "\n";
                return 0;
            } else {
                std::cerr << "Unknown option: " << arg << "\n";
                std::cerr << "Use --help for usage information.\n";
                return 1;
            }
        }

        // Configure logging
        if (log_level == "debug") {
            spdlog::set_level(spdlog::level::debug);
        } else if (log_level == "info") {
            spdlog::set_level(spdlog::level::info);
        } else if (log_level == "warn") {
            spdlog::set_level(spdlog::level::warn);
        } else if (log_level == "error") {
            spdlog::set_level(spdlog::level::err);
        }

        hegadb::logging::info("Starting HEGA-DB Local Execution Engine");
        hegadb::logging::info("Data directory: {}", data_dir);

        // ====================================================================
        // Initialize Catalog from disk
        // ====================================================================

        hegadb::logging::info("Loading catalog from: {}", catalog_dir);
        auto catalog = hegadb::execution::CatalogLoader::loadFromDirectory(catalog_dir);

        // ====================================================================
        // Initialize Storage
        // ====================================================================
        
        std::shared_ptr<hegadb::server::storage::StorageManager> storage;
        
        try {
            hegadb::logging::info("Initializing file-based storage");
            storage = hegadb::server::storage::StorageManager::createFileStorage(data_dir, catalog_dir);
            hegadb::logging::info("Storage initialized successfully");
        } catch (std::exception const& e) {
            hegadb::logging::warn("Could not initialize file storage: {}", e.what());
            hegadb::logging::warn("Storage features will be limited");
            
            // Create a mock storage for testing
            class MockStorage : public hegadb::server::storage::StorageManager {
            public:
                std::unique_ptr<hegadb::server::storage::EncryptedTable> openTable(
                    std::string const& table_name) override {
                    throw std::runtime_error("Mock storage: table not found: " + table_name);
                }
            };
            
            storage = std::make_shared<MockStorage>();
        }

        // ====================================================================
        // Initialize Encryption
        // ====================================================================
        
        std::string keys_dir = home_dir + "/.hegadb/keys";
        hegadb::logging::info("Initializing encryption from: {}", keys_dir);
        auto encryption = std::make_shared<hegadb::crypto::CompositeEncryption>(keys_dir);
        hegadb::logging::info("Encryption initialized successfully");

        // ====================================================================
        // Create Local Executor
        // ====================================================================
        
        hegadb::logging::info("Creating local executor");
        auto executor = std::make_shared<hegadb::execution::LocalExecutor>(
            catalog,
            storage,
            encryption
        );

        // ====================================================================
        // Start REPL
        // ====================================================================
        
        hegadb::execution::REPLConfig repl_config;
        repl_config.show_timing = true;
        repl_config.output_format = hegadb::execution::DisplayFormat::Table;

        hegadb::execution::ExecutionREPL repl(executor, repl_config);
        
        return repl.run();

    } catch (std::exception const& error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 1;
    }
}
