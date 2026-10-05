#include <hegadb/tools/setup/EmployeeDataGenerator.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/crypto/Plaintext.h>
#include <hegadb/crypto/BFVEncryption.h>

#include <stdexcept>
#include <algorithm>

namespace hegadb::tools::setup {

EmployeeDataGenerator::EmployeeDataGenerator(
    std::shared_ptr<crypto::CompositeEncryption> encryption)
    : encryption_(std::move(encryption))
{
}

catalog::TableSchema EmployeeDataGenerator::getEmployeesSchema() {
    std::vector<catalog::ColumnSchema> columns = {
        { "id", types::DataType::Integer, false, 0 },
        { "age", types::DataType::Integer, false, 1 },
        { "salary", types::DataType::Integer, false, 2 },
        { "dept", types::DataType::String, false, 3 }
    };

    return catalog::TableSchema("EMPLOYEES", std::move(columns));
}

std::vector<EmployeeRecord> EmployeeDataGenerator::generateSampleEmployees() {
    return {
        {1, 25, 75000, "Engineering"},
        {2, 32, 62000, "Sales"},
        {3, 30, 85000, "Engineering"},
        {4, 45, 95000, "Management"},
        {5, 28, 80000, "Engineering"},
        {6, 35, 68000, "Sales"},
        {7, 29, 72000, "Marketing"},
        {8, 42, 90000, "Management"},
        {9, 26, 65000, "Marketing"},
        {10, 38, 78000, "Sales"}
    };
}

crypto::OpenFHECiphertext EmployeeDataGenerator::encryptColumn(
    std::vector<std::string> const& values,
    types::DataType type)
{
    if (values.empty()) {
        throw std::runtime_error("Cannot encrypt empty column");
    }

    // Access the BFVEncryption engine directly through the context + key manager
    // so we can use the vector-packing methods.
    crypto::BFVEncryption bfv(
        encryption_->getContext(),
        encryption_->getKeyManager()
    );

    std::unique_ptr<crypto::Ciphertext> ciphertext;

    if (type == types::DataType::Integer || type == types::DataType::Boolean) {
        // Pack all row values into a single BFV ciphertext (one value per slot)
        std::vector<int64_t> int_values;
        int_values.reserve(values.size());
        for (auto const& v : values) {
            int_values.push_back(std::stoll(v));
        }
        ciphertext = bfv.encryptIntegerVector(int_values);

    } else if (type == types::DataType::String) {
        // Pack all row strings into a single BFV ciphertext using stride layout
        ciphertext = bfv.encryptStringVector(values, 32 /* max_string_len */);

    } else {
        // Fallback: encrypt first value only (Float / unknown)
        auto plaintext = parseValue(values[0], type);
        ciphertext = encryption_->encrypt(plaintext);
    }

    // Downcast to OpenFHECiphertext
    auto* openfhe_ct = dynamic_cast<crypto::OpenFHECiphertext*>(ciphertext.get());
    if (!openfhe_ct) {
        throw std::runtime_error("Expected OpenFHECiphertext");
    }

    return *openfhe_ct;
}

crypto::Plaintext EmployeeDataGenerator::parseValue(
    std::string const& value,
    types::DataType type)
{
    switch (type) {
        case types::DataType::Integer: {
            try {
                long long int_val = std::stoll(value);
                return crypto::Plaintext(int_val);
            } catch (...) {
                throw std::runtime_error("Invalid integer value: " + value);
            }
        }

        case types::DataType::String: {
            return crypto::Plaintext(value);
        }

        case types::DataType::Boolean: {
            std::string lower_value = value;
            std::transform(lower_value.begin(), lower_value.end(), 
                          lower_value.begin(), ::tolower);
            
            if (lower_value == "true" || lower_value == "1" || lower_value == "yes") {
                return crypto::Plaintext(true);
            } else if (lower_value == "false" || lower_value == "0" || lower_value == "no") {
                return crypto::Plaintext(false);
            } else {
                throw std::runtime_error("Invalid boolean value: " + value);
            }
        }

        case types::DataType::Float: {
            // Float stored as string in Variant system
            return crypto::Plaintext(value);
        }

        default:
            throw std::runtime_error("Unsupported data type");
    }
}

} // namespace hegadb::tools::setup
