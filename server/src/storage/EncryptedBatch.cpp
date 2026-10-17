#include <hegadb/logging/Logger.h>
#include <hegadb/server/storage/EncryptedBatch.h>
#include <stdexcept>

namespace hegadb::server::storage {

// EncryptedBatch is currently a simple struct, but we can add utility methods here

bool EncryptedBatch::hasColumn(std::string const& column_name) const {
    return columns.find(column_name) != columns.end();
}

crypto::OpenFHECiphertext const& EncryptedBatch::getColumn(
    std::string const& column_name) const 
{
    auto it = columns.find(column_name);
    if (it == columns.end()) {
        hegadb::logging::warn("Attempted to access non-existent column '{}' in EncryptedBatch", column_name);
        throw std::out_of_range("Column not found in batch: " + column_name);
    }
    return it->second;
}

size_t EncryptedBatch::getColumnCount() const {
    return columns.size();
}

bool EncryptedBatch::isEmpty() const {
    return row_count == 0 || columns.empty();
}

void EncryptedBatch::clear() {
    row_count = 0;
    columns.clear();
}

} // namespace hegadb::server::storage
