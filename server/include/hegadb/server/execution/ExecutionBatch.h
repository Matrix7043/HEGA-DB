#ifndef EXECUTION_BATCH_H
#define EXECUTION_BATCH_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace hegadb::server::execution {

/**
 * @brief Represents a batch of encrypted rows organized by column
 *
 * This is the primary data unit between storage and operators.
 * Data is organized column-wise with each column containing encrypted values.
 */
class ExecutionBatch {
public:
    ExecutionBatch() = default;

    /**
     * @brief Set the number of rows in this batch
     */
    void setRowCount(size_t count);

    /**
     * @brief Get the number of rows in this batch
     */
    size_t getRowCount() const;

    /**
     * @brief Add an encrypted column to the batch
     */
    void addColumn(
        std::string const& column_name,
        crypto::OpenFHECiphertext ciphertext
    );

    /**
     * @brief Get an encrypted column by name
     * @throws std::out_of_range if column doesn't exist
     */
    crypto::OpenFHECiphertext const& getColumn(std::string const& column_name) const;

    /**
     * @brief Check if column exists
     */
    bool hasColumn(std::string const& column_name) const;

    /**
     * @brief Get all column names
     */
    std::vector<std::string> getColumnNames() const;

    /**
     * @brief Check if batch is empty
     */
    bool empty() const;

    /**
     * @brief Get the number of columns in this batch
     */
    size_t getColumnCount() const;

    /**
     * @brief Remove a column from the batch
     * @return true if column was removed, false if it didn't exist
     */
    bool removeColumn(std::string const& column_name);

    /**
     * @brief Clear all data from the batch
     */
    void clear();

private:
    size_t row_count_ = 0;
    std::map<std::string, crypto::OpenFHECiphertext> columns_;
};

} // namespace hegadb::server::execution

#endif // EXECUTION_BATCH_H
