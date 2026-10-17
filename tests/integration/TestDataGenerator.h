#ifndef TEST_DATA_GENERATOR_H
#define TEST_DATA_GENERATOR_H

#include "InMemoryStorageManager.h"
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/types/DataType.h>

#include <memory>
#include <string>
#include <vector>

namespace hegadb::test {

/**
 * @brief Generates and encrypts test data for integration testing
 */
class TestDataGenerator {
public:
    TestDataGenerator(
        std::shared_ptr<crypto::CompositeEncryption> encryption
    );

    /**
     * @brief Generate STUDENTS table with sample encrypted data
     * 
     * Creates a STUDENTS table with columns: NAME, CLASS, GRADE
     * Populates with 8 sample students
     * 
     * @param storage In-memory storage to populate
     * @param catalog Catalog to add schema to
     */
    void generateStudentsTable(
        InMemoryStorageManager& storage,
        std::shared_ptr<catalog::InMemoryCatalog> catalog
    );

    /**
     * @brief Get the STUDENTS table schema (for reference)
     */
    static catalog::TableSchema getStudentsSchema();

private:
    /**
     * @brief Get sample student data (plaintext)
     */
    static std::vector<std::vector<std::string>> getSampleStudentData();

    /**
     * @brief Encrypt a column of data
     */
    crypto::OpenFHECiphertext encryptColumn(
        std::vector<std::string> const& values,
        types::DataType type
    );

    /**
     * @brief Parse and convert string value to Plaintext
     */
    crypto::Plaintext parseValue(
        std::string const& value,
        types::DataType type
    );

    std::shared_ptr<crypto::CompositeEncryption> encryption_;
};

} // namespace hegadb::test

#endif // TEST_DATA_GENERATOR_H
