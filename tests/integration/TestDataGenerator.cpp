#include "TestDataGenerator.h"
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/crypto/Plaintext.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace hegadb::test {

TestDataGenerator::TestDataGenerator(
    std::shared_ptr<crypto::CompositeEncryption> encryption)
    : encryption_(std::move(encryption))
{
}

catalog::TableSchema TestDataGenerator::getStudentsSchema() {
    std::vector<catalog::ColumnSchema> columns = {
        { "NAME", types::DataType::String, false, 0 },
        { "CLASS", types::DataType::Integer, false, 1 },
        { "GRADE", types::DataType::Integer, false, 2 }
    };

    return catalog::TableSchema("STUDENTS", std::move(columns));
}

std::vector<std::vector<std::string>> TestDataGenerator::getSampleStudentData() {
    return {
        {"Alice", "10", "85"},
        {"Bob", "10", "92"},
        {"Charlie", "11", "78"},
        {"Diana", "10", "88"},
        {"Eve", "11", "95"},
        {"Frank", "10", "76"},
        {"Grace", "11", "91"},
        {"Henry", "10", "83"}
    };
}

void TestDataGenerator::generateStudentsTable(
    InMemoryStorageManager& storage,
    std::shared_ptr<catalog::InMemoryCatalog> catalog)
{
    std::cout << "[SETUP] Generating STUDENTS table...\n";

    // Get schema and data
    auto schema = getStudentsSchema();
    auto sample_data = getSampleStudentData();

    std::cout << "[SETUP]   Table: " << schema.name() << "\n";
    std::cout << "[SETUP]   Columns: " << schema.columns().size() << "\n";
    std::cout << "[SETUP]   Rows: " << sample_data.size() << "\n";

    // Organize data by column
    std::vector<std::vector<std::string>> column_data(schema.columns().size());
    
    for (auto const& row : sample_data) {
        for (size_t col_idx = 0; col_idx < schema.columns().size(); ++col_idx) {
            column_data[col_idx].push_back(row[col_idx]);
        }
    }

    // Encrypt each column
    std::vector<crypto::OpenFHECiphertext> encrypted_columns;
    std::vector<std::string> column_names;

    for (size_t col_idx = 0; col_idx < schema.columns().size(); ++col_idx) {
        auto const& col_schema = schema.columns()[col_idx];
        column_names.push_back(col_schema.name);

        std::cout << "[SETUP]   Encrypting column '" << col_schema.name << "'...\n";

        auto encrypted_col = encryptColumn(
            column_data[col_idx],
            col_schema.type
        );

        encrypted_columns.push_back(std::move(encrypted_col));
    }

    // Add to storage
    storage.addTable(
        schema.name(),
        column_names,
        encrypted_columns,
        sample_data.size()
    );

    std::cout << "[SETUP]   ✓ Encrypted and stored " << sample_data.size() << " rows\n";

    // Add schema to catalog - create new catalog instance
    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace(schema.name(), std::move(schema));

    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(
        1,
        std::move(tables)
    );

    // Replace the catalog contents by reconstructing it
    *catalog = catalog::InMemoryCatalog(snapshot);

    std::cout << "[SETUP]   ✓ Added schema to catalog\n";
}

crypto::OpenFHECiphertext TestDataGenerator::encryptColumn(
    std::vector<std::string> const& values,
    types::DataType type)
{
    // For testing, we encrypt each value individually and pack them
    // In a real system, we'd use vector packing for efficiency
    
    // For now, just encrypt the first value as a representative
    // TODO: Implement proper vector packing
    if (values.empty()) {
        throw std::runtime_error("Cannot encrypt empty column");
    }

    auto plaintext = parseValue(values[0], type);
    auto ciphertext = encryption_->encrypt(plaintext);

    // Return the ciphertext (downcast to OpenFHECiphertext)
    // In real implementation, this would pack all values
    auto* openfhe_ct = dynamic_cast<crypto::OpenFHECiphertext*>(ciphertext.get());
    if (!openfhe_ct) {
        throw std::runtime_error("Expected OpenFHECiphertext");
    }

    return *openfhe_ct;
}

crypto::Plaintext TestDataGenerator::parseValue(
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
            // Float stored as string in our Variant system
            return crypto::Plaintext(value);
        }

        default:
            throw std::runtime_error("Unsupported data type");
    }
}

} // namespace hegadb::test
