#include <hegadb/execution/DataLoader.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/crypto/Plaintext.h>
#include <hegadb/server/storage/segment_writer.h>
#include <hegadb/server/storage/table_manifest.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace hegadb::execution {

DataLoader::DataLoader(
    std::shared_ptr<server::storage::StorageManager> storage,
    std::shared_ptr<crypto::CompositeEncryption> encryption)
    : storage_(std::move(storage))
    , encryption_(std::move(encryption))
{
}

LoadResult DataLoader::loadCSV(
    std::filesystem::path const& csv_path,
    catalog::TableSchema const& schema,
    CSVLoadConfig const& config)
{
    LoadResult result;
    result.success = false;
    result.table_name = schema.name();
    result.rows_loaded = 0;
    result.rows_failed = 0;

    logging::info("DataLoader: Loading CSV file: {}", csv_path.string());

    // Open CSV file
    std::ifstream file(csv_path);
    if (!file.is_open()) {
        result.error_message = "Failed to open file: " + csv_path.string();
        logging::error("DataLoader: {}", result.error_message);
        return result;
    }

    // Parse CSV and collect rows
    std::vector<std::vector<std::string>> rows;
    std::string line;
    size_t line_number = 0;

    // Skip header if configured
    if (config.has_header) {
        if (!std::getline(file, line)) {
            result.error_message = "Empty CSV file";
            logging::error("DataLoader: {}", result.error_message);
            return result;
        }
        line_number++;
    }

    // Read all data rows
    while (std::getline(file, line)) {
        line_number++;

        if (line.empty()) {
            continue;
        }

        auto fields = parseCSVLine(line, config.delimiter);

        if (!validateRowWidth(fields, schema.columns().size())) {
            result.rows_failed++;
            if (config.skip_errors) {
                logging::warn("DataLoader: Line {}: Column count mismatch (expected {}, got {})",
                    line_number, schema.columns().size(), fields.size());
                continue;
            } else {
                result.error_message = "Line " + std::to_string(line_number) +
                    ": Column count mismatch (expected " + std::to_string(schema.columns().size()) +
                    ", got " + std::to_string(fields.size()) + ")";
                logging::error("DataLoader: {}", result.error_message);
                return result;
            }
        }

        rows.push_back(std::move(fields));
    }

    file.close();

    logging::info("DataLoader: Parsed {} rows from CSV", rows.size());

    // Load the parsed data
    return loadFromMemory(schema.name(), schema, rows);
}

LoadResult DataLoader::loadFromMemory(
    std::string const& table_name,
    catalog::TableSchema const& schema,
    std::vector<std::vector<std::string>> const& rows)
{
    LoadResult result;
    result.success = false;
    result.table_name = table_name;
    result.rows_loaded = 0;
    result.rows_failed = 0;

    if (rows.empty()) {
        result.error_message = "No data to load";
        logging::warn("DataLoader: {}", result.error_message);
        result.success = true;  // Not an error, just empty
        return result;
    }

    try {
        logging::info("DataLoader: Encrypting and storing {} rows for table '{}'",
            rows.size(), table_name);

        // Create table directory
        // TODO: This needs to integrate with StorageManager's actual write API
        // For now, this is a placeholder showing the encryption logic
        
        // Group data by column
        std::vector<std::vector<crypto::Plaintext>> column_data(schema.columns().size());

        for (auto const& row : rows) {
            for (size_t col_idx = 0; col_idx < schema.columns().size(); ++col_idx) {
                auto const& col_schema = schema.columns()[col_idx];
                auto plaintext = parseValue(row[col_idx], col_schema.type);
                column_data[col_idx].push_back(std::move(plaintext));
            }
        }

        // Encrypt each column
        for (size_t col_idx = 0; col_idx < schema.columns().size(); ++col_idx) {
            auto const& col_schema = schema.columns()[col_idx];
            logging::debug("DataLoader: Encrypting column '{}' ({} values)",
                col_schema.name, column_data[col_idx].size());

            // TODO: Batch encrypt and write to segment files
            // This requires implementing a SegmentWriter wrapper or extending StorageManager
            // For now, we'll encrypt individually as a proof of concept

            std::vector<std::unique_ptr<crypto::Ciphertext>> encrypted_values;
            for (auto const& plaintext : column_data[col_idx]) {
                auto ciphertext = encryption_->encrypt(plaintext);
                encrypted_values.push_back(std::move(ciphertext));
            }

            logging::debug("DataLoader: Column '{}' encrypted successfully", col_schema.name);
        }

        result.rows_loaded = rows.size();
        result.success = true;

        logging::info("DataLoader: Successfully loaded {} rows into table '{}'",
            result.rows_loaded, table_name);

    } catch (std::exception const& error) {
        result.error_message = std::string("Loading failed: ") + error.what();
        logging::error("DataLoader: {}", result.error_message);
        return result;
    }

    return result;
}

std::vector<std::string> DataLoader::parseCSVLine(
    std::string const& line,
    char delimiter)
{
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // Toggle quote mode
            in_quotes = !in_quotes;
        } else if (c == delimiter && !in_quotes) {
            // End of field
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }

    // Add last field
    fields.push_back(field);

    return fields;
}

crypto::Plaintext DataLoader::parseValue(
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

        case types::DataType::Float: {
            // Float not directly supported in Variant, convert to string
            try {
                double float_val = std::stod(value);
                return crypto::Plaintext(std::to_string(float_val));
            } catch (...) {
                throw std::runtime_error("Invalid float value: " + value);
            }
        }

        case types::DataType::String: {
            return crypto::Plaintext(value);
        }

        case types::DataType::Boolean: {
            std::string lower_value = value;
            std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);
            
            if (lower_value == "true" || lower_value == "1" || lower_value == "yes") {
                return crypto::Plaintext(true);
            } else if (lower_value == "false" || lower_value == "0" || lower_value == "no") {
                return crypto::Plaintext(false);
            } else {
                throw std::runtime_error("Invalid boolean value: " + value);
            }
        }

        default:
            throw std::runtime_error("Unsupported data type");
    }
}

bool DataLoader::validateRowWidth(
    std::vector<std::string> const& row,
    size_t expected_columns)
{
    return row.size() == expected_columns;
}

} // namespace hegadb::execution
