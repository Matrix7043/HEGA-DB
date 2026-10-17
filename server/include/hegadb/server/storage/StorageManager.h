#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <hegadb/server/storage/EncryptedTable.h>
#include <memory>
#include <string>

namespace hegadb::server::storage {

class StorageManager {
public:
    StorageManager() = default;
    virtual ~StorageManager() = default;

    virtual std::unique_ptr<EncryptedTable> openTable(
        std::string const& table_name
    ) = 0;
    
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

