#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace hegadb::server::storage {

class StorageManager {
public:
    StorageManager() = default;
    virtual ~StorageManager() = default;

    virtual std::unique_ptr<EncryptedTable> openTable(
        std::string const& table_name)
        = 0;

    /**
     * @brief Create a new table in storage
     * @param schema Table schema including column definitions
     * @return true if successful, false if not supported or failed
     */
    virtual bool createTable(catalog::TableSchema const& /*schema*/)
    {
        return false;
    }

    /**
     * @brief Append an encrypted row to a table
     * @param table_name Name of the table
     * @param column_values Map of column name to single-slot encrypted ciphertext
     * @param crypto_context Shared pointer to OpenFHE crypto context
     * @return true if successful, false if not supported or failed
     */
    virtual bool appendRow(
        std::string const& /*table_name*/,
        std::unordered_map<std::string, crypto::OpenFHECiphertext> const& /*column_values*/,
        std::shared_ptr<crypto::OpenFHEContext> /*crypto_context*/)
    {
        return false;
    }

    /**
     * @brief Factory method to create a file-based storage manager
     * @param data_directory  Root directory containing table segment subdirectories
     * @param catalog_directory  Directory containing <TABLE>.manifest.json files
     * @return Unique pointer to StorageManager implementation
     */
    static std::unique_ptr<StorageManager> createFileStorage(
        std::string const& data_directory,
        std::string const& catalog_directory);
};

} // namespace hegadb::server::storage

#endif // STORAGE_MANAGER_H
