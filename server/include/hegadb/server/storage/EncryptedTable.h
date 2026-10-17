#ifndef ENCRYPTED_TABLE_H
#define ENCRYPTED_TABLE_H

#include <hegadb/server/storage/EncryptedBatch.h>
#include <optional>
#include <string>
#include <vector>

namespace hegadb::server::storage {

/**
 * @brief Interface for reading encrypted table data
 *
 * Abstracts physical storage details from the execution engine.
 * Provides batch-oriented access to encrypted data.
 */
class EncryptedTable {
public:
    virtual ~EncryptedTable() = default;

    virtual std::string const& getName() const = 0;

    virtual std::vector<std::string> getColumnNames() const = 0;

    virtual void open() = 0;

    virtual std::optional<EncryptedBatch> nextBatch() = 0;

    virtual void close() = 0;

    virtual size_t getRowCount() const = 0;
};

} // namespace hegadb::server::storage

#endif // ENCRYPTED_TABLE_H
