#ifndef HEGADB_EXECUTION_RESULT_DECRYPTOR_H
#define HEGADB_EXECUTION_RESULT_DECRYPTOR_H

#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <hegadb/types/DataType.h>

#include <memory>
#include <string>
#include <vector>

namespace hegadb::execution {

/**
 * @brief Decrypted row data
 */
struct DecryptedRow {
    std::vector<std::string> values;  // String representation of each column value
    bool matches_filter;              // True if row passed WHERE clause
};

/**
 * @brief Decrypted query result
 */
struct DecryptedResult {
    std::vector<std::string> column_names;
    std::vector<types::DataType> column_types;
    std::vector<DecryptedRow> rows;
    size_t total_rows_scanned;
};

/**
 * @brief Decrypts encrypted query results from server execution
 * 
 * ResultDecryptor:
 * 1. Takes encrypted ExecutionResult from server components
 * 2. Decrypts column ciphertexts
 * 3. Decrypts filter mask to determine which rows matched
 * 4. Converts decrypted values to human-readable strings
 */
class ResultDecryptor {
public:
    /**
     * @brief Constructor
     * @param encryption Encryption engine (needs private keys for decryption)
     */
    explicit ResultDecryptor(
        std::shared_ptr<crypto::CompositeEncryption> encryption
    );

    /**
     * @brief Decrypt execution result
     * @param result Encrypted execution result from server
     * @param column_names Names of columns in result
     * @param column_types Types of columns in result
     * @return Decrypted result with plaintext values
     */
    DecryptedResult decrypt(
        server::execution::ExecutionResult const& result,
        std::vector<std::string> const& column_names,
        std::vector<types::DataType> const& column_types
    );

private:
    /**
     * @brief Decrypt a single ciphertext and format as string
     */
    std::string decryptAndFormat(
        crypto::OpenFHECiphertext const& ciphertext,
        types::DataType type
    );

    /**
     * @brief Decrypt all row slots from a column ciphertext.
     *
     * Returns one string per row, reading each packed slot value individually.
     * Handles both the new vector-packed format and the legacy scalar format.
     */
    std::vector<std::string> decryptColumnSlots(
        crypto::OpenFHECiphertext const& ciphertext,
        types::DataType type,
        size_t row_count
    );

    /**
     * @brief Decrypt mask to determine which rows matched filter
     * @return Vector of booleans (true = row matched)
     */
    std::vector<bool> decryptMask(
        server::execution::EncryptedMask const& mask,
        size_t row_count
    );

    std::shared_ptr<crypto::CompositeEncryption> encryption_;
};

} // namespace hegadb::execution

#endif // HEGADB_EXECUTION_RESULT_DECRYPTOR_H
