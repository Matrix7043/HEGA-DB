#ifndef ENCRYPTED_BATCH_H
#define ENCRYPTED_BATCH_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <map>
#include <string>

namespace hegadb::server::storage {

struct EncryptedBatch {
    size_t row_count = 0;
    std::map<std::string, crypto::OpenFHECiphertext> columns;
    
    // Utility methods
    bool hasColumn(std::string const& column_name) const;
    crypto::OpenFHECiphertext const& getColumn(std::string const& column_name) const;
    size_t getColumnCount() const;
    bool isEmpty() const;
    void clear();
};

} // namespace hegadb::server::storage

#endif // ENCRYPTED_BATCH_H

