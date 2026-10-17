#include <hegadb/tools/setup/EmployeeDataGenerator.h>
#include <hegadb/tools/setup/FileStorageWriter.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/KeyManager.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <wordexp.h>

using namespace hegadb;
using namespace hegadb::tools::setup;

/**
 * @brief Expand shell variables like ~ in paths
 */
std::string expandPath(std::string const& path) {
    wordexp_t exp_result;
    wordexp(path.c_str(), &exp_result, 0);
    std::string expanded = exp_result.we_wordv[0];
    wordfree(&exp_result);
    return expanded;
}

/**
 * @brief Display usage banner
 */
void displayBanner() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║       HEGA-DB Database Setup Tool                 ║\n";
    std::cout << "║       Encrypted Database Initialization           ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

/**
 * @brief Display help message
 */
void displayHelp() {
    std::cout << "Usage: hegadb_setup [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --init-database     Initialize EMPLOYEES database with encrypted data\n";
    std::cout << "  --data-dir DIR      Data directory (default: ~/.hegadb/data)\n";
    std::cout << "  --catalog-dir DIR   Catalog directory (default: ~/.hegadb/catalog)\n";
    std::cout << "  --help              Display this help message\n";
    std::cout << "\n";
    std::cout << "Example:\n";
    std::cout << "  ./hegadb_setup --init-database\n";
    std::cout << "\n";
}

/**
 * @brief Initialize the EMPLOYEES database
 */
int initializeDatabase(std::string const& data_dir, std::string const& catalog_dir) {
    try {
        std::cout << "=== DATABASE INITIALIZATION ===\n\n";

        // Expand paths
        std::string data_path = expandPath(data_dir);
        std::string catalog_path = expandPath(catalog_dir);

        std::cout << "[INIT]    Data directory: " << data_path << "\n";
        std::cout << "[INIT]    Catalog directory: " << catalog_path << "\n\n";

        // Define table directory (data only — keys live separately)
        std::string table_dir = data_path + "/EMPLOYEES";
        std::string keys_path = expandPath("~/.hegadb/keys");

        // Initialize encryption using the dedicated keys directory
        std::cout << "[CRYPTO]  Initializing encryption with keys in: " << keys_path << "\n";
        auto encryption = std::make_shared<crypto::CompositeEncryption>(keys_path);
        std::cout << "[CRYPTO]  ✓ Keys initialized and saved\n\n";

        // Generate employee data
        std::cout << "[DATA]    Generating EMPLOYEES table...\n";
        EmployeeDataGenerator generator(encryption);
        
        auto schema = EmployeeDataGenerator::getEmployeesSchema();
        auto employees = EmployeeDataGenerator::generateSampleEmployees();

        std::cout << "[DATA]    Table: " << schema.name() << "\n";
        std::cout << "[DATA]    Columns: " << schema.columns().size() << "\n";
        std::cout << "[DATA]    Rows: " << employees.size() << "\n\n";

        // Organize data by column
        std::vector<std::vector<std::string>> column_data(schema.columns().size());
        
        for (auto const& emp : employees) {
            column_data[0].push_back(std::to_string(emp.id));
            column_data[1].push_back(std::to_string(emp.age));
            column_data[2].push_back(std::to_string(emp.salary));
            column_data[3].push_back(emp.dept);
        }

        // Encrypt each column
        std::cout << "[ENCRYPT] Encrypting columns...\n";
        std::vector<crypto::OpenFHECiphertext> encrypted_columns;
        std::vector<std::string> column_names;
        std::vector<types::DataType> column_types;

        for (size_t i = 0; i < schema.columns().size(); ++i) {
            auto const& col_schema = schema.columns()[i];
            column_names.push_back(col_schema.name);
            column_types.push_back(col_schema.type);

            std::cout << "[ENCRYPT]   Column: " << col_schema.name << " ... ";
            std::cout.flush();

            auto encrypted_col = generator.encryptColumn(
                column_data[i],
                col_schema.type
            );

            encrypted_columns.push_back(std::move(encrypted_col));
            std::cout << "✓\n";
        }

        std::cout << "[ENCRYPT] ✓ All columns encrypted\n\n";

        // Write to disk
        std::cout << "[STORAGE] Writing encrypted data to disk...\n";
        FileStorageWriter writer(data_path);
        
        writer.writeTable(
            schema.name(),
            column_names,
            column_types,
            encrypted_columns,
            employees.size()
        );

        std::cout << "\n[CATALOG] Writing schema metadata...\n";
        FileStorageWriter::writeManifest(schema, catalog_path, employees.size());
        std::cout << "[CATALOG] ✓ Manifest written to: " << catalog_path << "\n";

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════╗\n";
        std::cout << "║              ✓ SUCCESS                            ║\n";
        std::cout << "╠════════════════════════════════════════════════════╣\n";
        std::cout << "║  Database initialized successfully!               ║\n";
        std::cout << "║                                                    ║\n";
        std::cout << "║  Table: EMPLOYEES                                 ║\n";
        std::cout << "║  Rows:  " << employees.size() << "                                          ║\n";
        std::cout << "║                                                    ║\n";
        std::cout << "║  Data:    " << data_path << " ║\n";
        std::cout << "║  Catalog: " << catalog_path << " ║\n";
        std::cout << "╚════════════════════════════════════════════════════╝\n";
        std::cout << "\n";

        return 0;

    } catch (std::exception const& e) {
        std::cerr << "\n[ERROR]   " << e.what() << "\n";
        std::cerr << "[ERROR]   Database initialization failed\n\n";
        return 1;
    }
}

int main(int argc, char* argv[]) {
    displayBanner();

    // Default paths
    std::string data_dir = "~/.hegadb/data";
    std::string catalog_dir = "~/.hegadb/catalog";
    bool init_db = false;

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            displayHelp();
            return 0;
        } else if (arg == "--init-database") {
            init_db = true;
        } else if (arg == "--data-dir" && i + 1 < argc) {
            data_dir = argv[++i];
        } else if (arg == "--catalog-dir" && i + 1 < argc) {
            catalog_dir = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            std::cerr << "Use --help for usage information\n";
            return 1;
        }
    }

    if (!init_db) {
        std::cerr << "No action specified. Use --init-database to create the database.\n";
        std::cerr << "Use --help for more information.\n";
        return 1;
    }

    return initializeDatabase(data_dir, catalog_dir);
}
