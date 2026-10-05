#ifndef HEGADB_TOOLS_SETUP_EMPLOYEE_DATA_GENERATOR_H
#define HEGADB_TOOLS_SETUP_EMPLOYEE_DATA_GENERATOR_H

#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/types/DataType.h>

#include <memory>
#include <string>
#include <vector>

namespace hegadb::tools::setup {

/**
 * @brief Employee record structure (plaintext)
 */
struct EmployeeRecord {
    int id;
    int age;
    int salary;
    std::string dept;
};

/**
 * @brief Generates sample employee data for database setup
 */
class EmployeeDataGenerator {
public:
    EmployeeDataGenerator(
        std::shared_ptr<crypto::CompositeEncryption> encryption
    );

    /**
     * @brief Get the EMPLOYEES table schema
     */
    static catalog::TableSchema getEmployeesSchema();

    /**
     * @brief Generate 10 sample employee records
     */
    static std::vector<EmployeeRecord> generateSampleEmployees();

    /**
     * @brief Encrypt a column of values
     * @param values Plaintext values as strings
     * @param type Data type of the column
     * @return Encrypted column as single ciphertext
     */
    crypto::OpenFHECiphertext encryptColumn(
        std::vector<std::string> const& values,
        types::DataType type
    );

private:
    /**
     * @brief Parse a string value into Plaintext based on type
     */
    crypto::Plaintext parseValue(
        std::string const& value,
        types::DataType type
    );

    std::shared_ptr<crypto::CompositeEncryption> encryption_;
};

} // namespace hegadb::tools::setup

#endif // HEGADB_TOOLS_SETUP_EMPLOYEE_DATA_GENERATOR_H
