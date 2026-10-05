#ifndef HEGADB_EXECUTION_LOCAL_EXECUTOR_H
#define HEGADB_EXECUTION_LOCAL_EXECUTOR_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/types/DataType.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace hegadb::execution {

/**
 * @brief Result of a query execution containing decrypted data
 */
struct QueryResult {
    std::vector<std::string> column_names;
    std::vector<types::DataType> column_types;
    std::vector<std::vector<std::string>> rows;  // Each row is a vector of string-formatted values
    size_t total_rows_scanned;
    bool success;
    std::string error_message;
};

/**
 * @brief Orchestrates entire query execution pipeline in a single process
 * 
 * LocalExecutor integrates:
 * - Client-side: SQL parsing, validation, encryption
 * - Server-side: Query planning, execution on encrypted data
 * - Result decryption and formatting
 * 
 * This eliminates the need for network communication and provides
 * a standalone execution environment for testing and local use.
 */
class LocalExecutor {
public:
    /**
     * @brief Constructor
     * @param catalog Schema catalog
     * @param storage Storage manager for encrypted tables
     * @param encryption Encryption engine for client-side operations
     */
    LocalExecutor(
        std::shared_ptr<catalog::Catalog> catalog,
        std::shared_ptr<server::storage::StorageManager> storage,
        std::shared_ptr<crypto::CompositeEncryption> encryption
    );

    /**
     * @brief Execute a SQL query end-to-end
     * @param sql SQL query string
     * @return Query result with decrypted data
     */
    QueryResult execute(std::string const& sql);

    /**
     * @brief Get the catalog
     */
    catalog::Catalog& getCatalog();
    catalog::Catalog const& getCatalog() const;

    /**
     * @brief Get the storage manager
     */
    server::storage::StorageManager& getStorageManager();

    /**
     * @brief Get the encryption engine
     */
    crypto::CompositeEncryption& getEncryption();

private:
    std::shared_ptr<catalog::Catalog> catalog_;
    std::shared_ptr<server::storage::StorageManager> storage_;
    std::shared_ptr<crypto::CompositeEncryption> encryption_;
    std::shared_ptr<crypto::OpenFHEContext> crypto_context_;
};

} // namespace hegadb::execution

#endif // HEGADB_EXECUTION_LOCAL_EXECUTOR_H
