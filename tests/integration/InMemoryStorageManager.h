#ifndef INMEMORY_STORAGE_MANAGER_H
#define INMEMORY_STORAGE_MANAGER_H

#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <hegadb/server/storage/EncryptedBatch.h>
#include <hegadb/crypto/OpenFHECiphertext.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hegadb::test {

/**
 * @brief Stores encrypted table data in memory for testing
 */
struct InMemoryTableData {
    std::string table_name;
    std::vector<std::string> column_names;
    std::vector<crypto::OpenFHECiphertext> columns;  // One ciphertext per column (packed)
    size_t row_count;
};

/**
 * @brief In-memory implementation of EncryptedTable
 */
class InMemoryTable : public server::storage::EncryptedTable {
public:
    InMemoryTable(InMemoryTableData data);

    std::string const& getName() const override;
    std::vector<std::string> getColumnNames() const override;
    void open() override;
    std::optional<server::storage::EncryptedBatch> nextBatch() override;
    void close() override;
    size_t getRowCount() const override;

private:
    InMemoryTableData data_;
    bool is_open_ = false;
    bool batch_returned_ = false;
};

/**
 * @brief In-memory storage manager for testing
 * 
 * Stores all encrypted data in RAM without any file I/O.
 * Perfect for fast, isolated testing of query execution.
 */
class InMemoryStorageManager : public server::storage::StorageManager {
public:
    InMemoryStorageManager() = default;

    /**
     * @brief Add a table to in-memory storage
     * @param table_name Name of the table
     * @param column_names Names of columns
     * @param columns Encrypted column data (one ciphertext per column)
     * @param row_count Number of rows
     */
    void addTable(
        std::string const& table_name,
        std::vector<std::string> const& column_names,
        std::vector<crypto::OpenFHECiphertext> const& columns,
        size_t row_count
    );

    /**
     * @brief Open a table for reading
     */
    std::unique_ptr<server::storage::EncryptedTable> openTable(
        std::string const& table_name
    ) override;

    /**
     * @brief Check if table exists
     */
    bool hasTable(std::string const& table_name) const;

    /**
     * @brief Clear all tables
     */
    void clear();

private:
    std::unordered_map<std::string, InMemoryTableData> tables_;
};

} // namespace hegadb::test

#endif // INMEMORY_STORAGE_MANAGER_H
