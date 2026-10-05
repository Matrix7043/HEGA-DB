#ifndef HEGADB_EXECUTION_DATA_LOADER_H
#define HEGADB_EXECUTION_DATA_LOADER_H

#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/types/DataType.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace hegadb::execution {

/**
 * @brief Configuration for CSV loading
 */
struct CSVLoadConfig {
    char delimiter = ',';
    bool has_header = true;
    size_t batch_size = 1000;  // Rows per encrypted batch
    bool skip_errors = false;
};

/**
 * @brief Result of a data loading operation
 */
struct LoadResult {
    bool success;
    std::string table_name;
    size_t rows_loaded;
    size_t rows_failed;
    std::string error_message;
};

/**
 * @brief Loads plaintext data (CSV) and stores it encrypted
 * 
 * DataLoader:
 * 1. Reads CSV files
 * 2. Parses and validates data according to schema
 * 3. Encrypts each column using CompositeEncryption
 * 4. Writes encrypted data to storage using segment format
 */
class DataLoader {
public:
    /**
     * @brief Constructor
     * @param storage Storage manager for writing encrypted data
     * @param encryption Encryption engine
     */
    DataLoader(
        std::shared_ptr<server::storage::StorageManager> storage,
        std::shared_ptr<crypto::CompositeEncryption> encryption
    );

    /**
     * @brief Load CSV file into encrypted storage
     * @param csv_path Path to CSV file
     * @param schema Table schema defining structure
     * @param config Load configuration
     * @return Load result with statistics
     */
    LoadResult loadCSV(
        std::filesystem::path const& csv_path,
        catalog::TableSchema const& schema,
        CSVLoadConfig const& config = CSVLoadConfig{}
    );

    /**
     * @brief Load data from in-memory rows
     * @param table_name Table name
     * @param schema Table schema
     * @param rows Vector of rows (each row is vector of string values)
     * @return Load result
     */
    LoadResult loadFromMemory(
        std::string const& table_name,
        catalog::TableSchema const& schema,
        std::vector<std::vector<std::string>> const& rows
    );

private:
    /**
     * @brief Parse a CSV line respecting quotes and delimiters
     */
    std::vector<std::string> parseCSVLine(
        std::string const& line,
        char delimiter
    );

    /**
     * @brief Convert string value to typed value and encrypt
     */
    crypto::Plaintext parseValue(
        std::string const& value,
        types::DataType type
    );

    /**
     * @brief Validate row has correct number of columns
     */
    bool validateRowWidth(
        std::vector<std::string> const& row,
        size_t expected_columns
    );

    std::shared_ptr<server::storage::StorageManager> storage_;
    std::shared_ptr<crypto::CompositeEncryption> encryption_;
};

} // namespace hegadb::execution

#endif // HEGADB_EXECUTION_DATA_LOADER_H
