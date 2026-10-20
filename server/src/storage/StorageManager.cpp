#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <hegadb/server/storage/SegmentReader.h>
#include <hegadb/server/storage/SegmentWriter.h>
#include <hegadb/server/storage/StorageFormat.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/TableManifest.h>

#include <openfhe.h>
#include <cstring>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace hegadb::server::storage {

/**
 * @brief Production implementation of StorageManager
 *
 * Manages file-based storage of encrypted tables using:
 * - Table manifests (JSON metadata)
 * - Segment files (binary encrypted data)
 * - Column-oriented storage format
 */
class FileStorageManager : public StorageManager {
private:
    std::filesystem::path data_directory_;
    std::filesystem::path catalog_directory_;

    /**
     * @brief Implementation of EncryptedTable that reads from segment files
     */
    class SegmentBasedTable : public EncryptedTable {
    private:
        TableManifest manifest_;
        std::filesystem::path table_directory_;
        std::unordered_map<std::string, std::unique_ptr<SegmentReader>> segment_readers_;
        bool is_open_ = false;
        uint64_t current_block_ = 0;
        uint64_t total_blocks_ = 0;

    public:
        SegmentBasedTable(TableManifest manifest, std::filesystem::path table_dir)
            : manifest_(std::move(manifest))
            , table_directory_(std::move(table_dir))
        {
            // Determine total blocks from segments
            for (auto const& [col_name, seg_meta] : manifest_.segments) {
                if (total_blocks_ == 0) {
                    total_blocks_ = seg_meta.total_blocks;
                } else if (total_blocks_ != seg_meta.total_blocks) {
                    hegadb::logging::error(
                        "Inconsistent block counts across segments in table '{}': segment '{}' has {}, expected {}",
                        manifest_.table_name, col_name, seg_meta.total_blocks, total_blocks_);
                    throw std::runtime_error(
                        "Inconsistent block counts across segments in table: " + manifest_.table_name);
                }
            }
            hegadb::logging::debug(
                "Initialized SegmentBasedTable for '{}': {} columns, {} total blocks, {} total records",
                manifest_.table_name, manifest_.columns.size(), total_blocks_, manifest_.total_records);
        }

        std::string const& getName() const override
        {
            return manifest_.table_name;
        }

        std::vector<std::string> getColumnNames() const override
        {
            std::vector<std::string> names;
            names.reserve(manifest_.columns.size());
            for (auto const& col : manifest_.columns) {
                names.push_back(col.name);
            }
            return names;
        }

        void open() override
        {
            if (is_open_) {
                hegadb::logging::debug("Table '{}' is already open", manifest_.table_name);
                return;
            }

            hegadb::logging::info("Opening segment readers for table '{}' from '{}'",
                                 manifest_.table_name, table_directory_.string());

            // Open segment readers for each column
            for (auto const& col : manifest_.columns) {
                auto seg_it = manifest_.segments.find(col.name);
                if (seg_it == manifest_.segments.end()) {
                    hegadb::logging::error("No segment metadata found in manifest for column '{}' in table '{}'",
                                           col.name, manifest_.table_name);
                    throw std::runtime_error(
                        "No segment found for column: " + col.name);
                }

                auto segment_path = table_directory_ / seg_it->second.file;
                hegadb::logging::debug("Opening segment file for column '{}': '{}'", col.name, segment_path.string());
                auto reader = std::make_unique<SegmentReader>(segment_path.string());

                // Read and validate header
                auto header_result = reader->read_header();
                if (!header_result.has_value()) {
                    hegadb::logging::error("Failed to read segment header for column '{}' in table '{}' at '{}'",
                                           col.name, manifest_.table_name, segment_path.string());
                    throw std::runtime_error(
                        "Failed to read segment header for column: " + col.name);
                }

                segment_readers_[col.name] = std::move(reader);
            }

            current_block_ = 0;
            is_open_ = true;
            hegadb::logging::info("Successfully opened table '{}' (readers initialized for {} columns)",
                                 manifest_.table_name, segment_readers_.size());
        }

        std::optional<EncryptedBatch> nextBatch() override
        {
            if (!is_open_) {
                hegadb::logging::error("Attempted nextBatch() on un-opened table '{}'", manifest_.table_name);
                throw std::runtime_error("Table not opened: " + manifest_.table_name);
            }

            // Check if we've read all blocks
            if (current_block_ >= total_blocks_) {
                hegadb::logging::debug("Reached end of data blocks for table '{}' (block {}/{})",
                                      manifest_.table_name, current_block_, total_blocks_);
                return std::nullopt;
            }

            hegadb::logging::trace("Reading block {}/{} for table '{}'",
                                  current_block_ + 1, total_blocks_, manifest_.table_name);

            // Read one block from each column's segment
            EncryptedBatch batch;
            std::vector<std::byte> payload_buffer;

            for (auto const& col : manifest_.columns) {
                auto& reader = segment_readers_.at(col.name);

                payload_buffer.clear();
                auto block_header_result = reader->read_next_block(payload_buffer);

                if (!block_header_result.has_value()) {
                    hegadb::logging::error("Failed to read block {} for column '{}' in table '{}'",
                                           current_block_, col.name, manifest_.table_name);
                    throw std::runtime_error(
                        "Failed to read block from column: " + col.name);
                }

                auto const& block_header = *block_header_result;

                // Set batch row count from first column
                if (batch.row_count == 0) {
                    batch.row_count = block_header.active_rows;
                } else if (batch.row_count != block_header.active_rows) {
                    hegadb::logging::error("Mismatched row counts across columns in block {} of table '{}': expected {}, got {}",
                                           current_block_, manifest_.table_name, batch.row_count, block_header.active_rows);
                    throw std::runtime_error(
                        "Mismatched row counts across columns in block " + std::to_string(current_block_));
                }

                // Deserialize the ciphertext from payload
                std::string payload_str(
                    reinterpret_cast<char const*>(payload_buffer.data()),
                    payload_buffer.size());

                try {
                    // Deserialize OpenFHE ciphertext directly (no HGCT wrapper)
                    std::istringstream blob_stream(payload_str, std::ios::in | std::ios::binary);
                    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> openfhe_ct;
                    lbcrypto::Serial::Deserialize(openfhe_ct, blob_stream, lbcrypto::SerType::BINARY);

                    if (!openfhe_ct) {
                        hegadb::logging::error("Deserialized ciphertext is null for column '{}' in block {}",
                                               col.name, current_block_);
                        throw std::runtime_error("Deserialized ciphertext is null");
                    }

                    // Determine scheme type and data type from column metadata
                    crypto::SchemeType scheme_type = (col.scheme == "CKKS") ? crypto::SchemeType::CKKS : crypto::SchemeType::BFV;

                    types::DataType data_type;
                    if (col.type == "INTEGER") {
                        data_type = types::DataType::Integer;
                    } else if (col.type == "STRING") {
                        data_type = types::DataType::String;
                    } else if (col.type == "FLOAT") {
                        data_type = types::DataType::Float;
                    } else if (col.type == "BOOLEAN") {
                        data_type = types::DataType::Boolean;
                    } else {
                        hegadb::logging::error("Unknown data type '{}' for column '{}' in table '{}'",
                                               col.type, col.name, manifest_.table_name);
                        throw std::runtime_error("Unknown data type: " + col.type);
                    }

                    // Wrap in OpenFHECiphertext with correct metadata.
                    // For String: stores (row_count * 1000 + stride) where stride=32
                    //             matches the encoding in BFVEncryption::encryptStringVector
                    // For Integer/Boolean/Float: stringLength must be 0 per OpenFHECiphertext specification
                    size_t meta_length = 0;
                    if (data_type == types::DataType::String) {
                        constexpr size_t STRING_STRIDE = 32;
                        meta_length = batch.row_count * 1000 + STRING_STRIDE;
                    }

                    auto ciphertext = crypto::OpenFHECiphertext(
                        openfhe_ct,
                        scheme_type,
                        data_type,
                        meta_length);

                    batch.columns.emplace(col.name, std::move(ciphertext));
                } catch (std::exception const& e) {
                    hegadb::logging::error("Failed to deserialize ciphertext for column '{}' in block {} of table '{}': {}",
                                           col.name, current_block_, manifest_.table_name, e.what());
                    throw std::runtime_error(
                        "Failed to deserialize ciphertext for column " + col.name + ": " + e.what());
                }
            }

            hegadb::logging::trace("Successfully read batch {} for table '{}': {} rows, {} columns",
                                  current_block_, manifest_.table_name, batch.row_count, batch.columns.size());

            current_block_++;
            return batch;
        }

        void close() override
        {
            if (!is_open_) {
                return;
            }

            hegadb::logging::info("Closing segment readers for table '{}'", manifest_.table_name);
            segment_readers_.clear();
            current_block_ = 0;
            is_open_ = false;
        }

        size_t getRowCount() const override
        {
            return manifest_.total_records;
        }
    };

public:
    FileStorageManager(std::string data_directory, std::string catalog_directory)
        : data_directory_(std::move(data_directory))
        , catalog_directory_(std::move(catalog_directory))
    {
        hegadb::logging::info("Initializing FileStorageManager with data_directory='{}', catalog_directory='{}'",
                             data_directory_.string(), catalog_directory_.string());

        if (!std::filesystem::exists(data_directory_)) {
            hegadb::logging::error("Storage data directory does not exist: '{}'", data_directory_.string());
            throw std::runtime_error(
                "Storage directory does not exist: " + data_directory_.string());
        }

        if (!std::filesystem::is_directory(data_directory_)) {
            hegadb::logging::error("Storage path is not a directory: '{}'", data_directory_.string());
            throw std::runtime_error(
                "Storage path is not a directory: " + data_directory_.string());
        }

        if (!std::filesystem::exists(catalog_directory_)) {
            hegadb::logging::error("Catalog directory does not exist: '{}'", catalog_directory_.string());
            throw std::runtime_error(
                "Catalog directory does not exist: " + catalog_directory_.string());
        }

        hegadb::logging::info("FileStorageManager successfully initialized");
    }

    std::unique_ptr<EncryptedTable> openTable(
        std::string const& table_name) override
    {
        hegadb::logging::info("openTable requested for table '{}'", table_name);

        // Construct path to table directory
        auto table_dir = data_directory_ / table_name;

        if (!std::filesystem::exists(table_dir)) {
            hegadb::logging::error("Table directory does not exist: '{}'", table_dir.string());
            throw std::runtime_error(
                "Table directory does not exist: " + table_dir.string());
        }

        // Load table manifest from the catalog directory
        auto manifest_path = catalog_directory_ / (table_name + ".manifest.json");
        hegadb::logging::debug("Deserializing table manifest from '{}'", manifest_path.string());
        auto manifest_result = TableManifest::deserialize(manifest_path.string());

        if (!manifest_result.has_value()) {
            hegadb::logging::error("Failed to load table manifest from '{}'", manifest_path.string());
            throw std::runtime_error(
                "Failed to load table manifest: " + manifest_path.string());
        }

        auto manifest = std::move(*manifest_result);

        // Verify table name matches
        if (manifest.table_name != table_name) {
            hegadb::logging::error("Table name mismatch in manifest '{}': expected '{}', got '{}'",
                                   manifest_path.string(), table_name, manifest.table_name);
            throw std::runtime_error(
                "Table name mismatch: expected " + table_name + ", got " + manifest.table_name);
        }

        hegadb::logging::info("Successfully opened table object for '{}' from directory '{}'",
                             table_name, table_dir.string());

        // Create and return table implementation
        return std::make_unique<SegmentBasedTable>(
            std::move(manifest),
            table_dir);
    }

    bool createTable(catalog::TableSchema const& schema) override
    {
        hegadb::logging::info("[STORAGE] createTable requested for '{}'", schema.name());

        auto table_dir = data_directory_ / schema.name();
        if (std::filesystem::exists(table_dir)) {
            hegadb::logging::error("[STORAGE] Table directory already exists: '{}'", table_dir.string());
            throw std::runtime_error("Table already exists: " + schema.name());
        }

        std::filesystem::create_directories(table_dir);

        // Build manifest
        TableManifest manifest;
        manifest.table_name = schema.name();
        manifest.total_records = 0;

        for (auto const& col : schema.columns()) {
            ColumnMetadata col_meta;
            col_meta.name = col.name;
            switch (col.type) {
            case types::DataType::Integer: col_meta.type = "INTEGER"; break;
            case types::DataType::String:  col_meta.type = "STRING"; break;
            case types::DataType::Boolean: col_meta.type = "BOOLEAN"; break;
            case types::DataType::Float:   col_meta.type = "FLOAT"; break;
            default: col_meta.type = "UNKNOWN"; break;
            }

            bool is_bfv = (col.type != types::DataType::Float);
            col_meta.scheme = is_bfv ? "BFV" : "CKKS";
            col_meta.slot_index = static_cast<uint32_t>(col.ordinal);
            manifest.columns.push_back(col_meta);

            SegmentMetadata seg_meta;
            seg_meta.file = col.name + ".seg";
            seg_meta.poly_N = 16384;
            seg_meta.param_scale_or_p = is_bfv ? 65537 : (1ULL << 40);
            seg_meta.rows_per_block = 0;
            seg_meta.total_blocks = 0;
            manifest.segments[col.name] = seg_meta;

            // Write initial segment header
            SegmentFileHeader seg_header{};
            if (is_bfv) {
                std::memcpy(seg_header.magic_bytes, BFV_MAGIC, 8);
            } else {
                std::memcpy(seg_header.magic_bytes, CKKS_MAGIC, 8);
            }
            seg_header.version = 1;
            seg_header.column_count = 1;
            seg_header.poly_degree_N = 16384;
            seg_header.modulus_count_L = 1;
            seg_header.rows_per_block = 0;
            seg_header.param_scale_or_p = is_bfv ? 65537 : (1ULL << 40);
            seg_header.total_blocks = 0;
            seg_header.total_row_count = 0;

            auto seg_path = table_dir / (col.name + ".seg");
            SegmentWriter writer(seg_path.string(), seg_header);
            writer.close();
        }

        auto manifest_path = catalog_directory_ / (schema.name() + ".manifest.json");
        auto ser_result = manifest.serialize(manifest_path.string());
        if (!ser_result) {
            hegadb::logging::error("[STORAGE] Failed to write manifest for '{}'", schema.name());
            throw std::runtime_error("Failed to write manifest: " + manifest_path.string());
        }

        hegadb::logging::info("[STORAGE] Successfully created table '{}' with {} columns",
                             schema.name(), schema.columns().size());
        return true;
    }

    bool appendRow(
        std::string const& table_name,
        std::unordered_map<std::string, crypto::OpenFHECiphertext> const& column_values,
        std::shared_ptr<crypto::OpenFHEContext> /*crypto_context*/) override
    {
        hegadb::logging::info("[STORAGE] appendRow requested for table '{}'", table_name);

        auto table_dir = data_directory_ / table_name;
        if (!std::filesystem::exists(table_dir)) {
            hegadb::logging::error("[STORAGE] Table directory does not exist: '{}'", table_dir.string());
            throw std::runtime_error("Table directory does not exist: " + table_dir.string());
        }

        auto manifest_path = catalog_directory_ / (table_name + ".manifest.json");
        auto manifest_res = TableManifest::deserialize(manifest_path.string());
        if (!manifest_res.has_value()) {
            hegadb::logging::error("[STORAGE] Could not load manifest for '{}'", table_name);
            throw std::runtime_error("Could not load manifest for: " + table_name);
        }

        auto manifest = std::move(*manifest_res);
        uint64_t current_rows = manifest.total_records;
        hegadb::logging::info("[STORAGE] Current rows for '{}': {}", table_name, current_rows);

        // Process each column
        for (auto const& col : manifest.columns) {
            auto it = column_values.find(col.name);
            if (it == column_values.end()) {
                hegadb::logging::error("[STORAGE] Missing value for column '{}' during insert into '{}'",
                                       col.name, table_name);
                throw std::runtime_error("Missing value for column: " + col.name);
            }

            auto const& incoming_wrapper = it->second;
            auto incoming_ct = incoming_wrapper.getCiphertext();
            auto seg_path = table_dir / (col.name + ".seg");

            lbcrypto::Ciphertext<lbcrypto::DCRTPoly> final_ct;

            if (current_rows == 0) {
                // Table has 0 rows: the slot-0 ciphertext becomes the column ciphertext
                final_ct = incoming_ct;
                hegadb::logging::debug("[STORAGE] Setting first row (slot 0) for column '{}'", col.name);
            } else {
                // Table has existing rows: read existing Block 0
                SegmentReader reader(seg_path.string());
                auto header_res = reader.read_header();
                if (!header_res.has_value()) {
                    throw std::runtime_error("Failed to read header from " + seg_path.string());
                }

                std::vector<std::byte> payload;
                auto block_res = reader.read_next_block(payload);
                if (!block_res.has_value()) {
                    throw std::runtime_error("Failed to read block 0 from " + seg_path.string());
                }

                std::string payload_str(reinterpret_cast<char const*>(payload.data()), payload.size());
                std::istringstream stream(payload_str, std::ios::in | std::ios::binary);
                lbcrypto::Ciphertext<lbcrypto::DCRTPoly> existing_ct;
                lbcrypto::Serial::Deserialize(existing_ct, stream, lbcrypto::SerType::BINARY);

                // Rotate incoming single-slot ciphertext by -current_rows
                auto cc = incoming_ct->GetCryptoContext();
                hegadb::logging::debug("[STORAGE] Rotating slot 0 to slot {} for column '{}'", current_rows, col.name);
                auto rotated_ct = cc->EvalRotate(incoming_ct, -static_cast<int32_t>(current_rows));

                // Add to existing column ciphertext
                hegadb::logging::debug("[STORAGE] Homomorphically adding rotated ciphertext to column '{}'", col.name);
                final_ct = cc->EvalAdd(existing_ct, rotated_ct);
            }

            // Serialize final_ct
            std::stringstream ss;
            lbcrypto::Serial::Serialize(final_ct, ss, lbcrypto::SerType::BINARY);
            std::string serialized = ss.str();

            // Prepare header
            auto cc = final_ct->GetCryptoContext();
            auto elem_params = cc->GetElementParams();
            uint32_t poly_N = elem_params->GetRingDimension();
            uint32_t modulus_count = elem_params->GetParams().size();
            bool is_bfv = (incoming_wrapper.getSchemeType() == crypto::SchemeType::BFV);
            uint64_t param_scale_or_p = is_bfv ?
                cc->GetCryptoParameters()->GetPlaintextModulus() :
                static_cast<uint64_t>(final_ct->GetScalingFactor());

            SegmentFileHeader seg_header{};
            if (is_bfv) {
                std::memcpy(seg_header.magic_bytes, BFV_MAGIC, 8);
            } else {
                std::memcpy(seg_header.magic_bytes, CKKS_MAGIC, 8);
            }
            seg_header.version = 1;
            seg_header.column_count = 1;
            seg_header.poly_degree_N = poly_N;
            seg_header.modulus_count_L = modulus_count;
            seg_header.rows_per_block = static_cast<uint32_t>(current_rows + 1);
            seg_header.param_scale_or_p = param_scale_or_p;
            seg_header.total_blocks = 1;
            seg_header.total_row_count = current_rows + 1;

            SegmentWriter writer(seg_path.string(), seg_header);
            BlockHeader bh{};
            bh.block_id = 0;
            bh.active_rows = static_cast<uint32_t>(current_rows + 1);
            bh.payload_byte_size = serialized.size();

            auto write_res = writer.write_block(
                bh,
                std::span<const std::byte>(
                    reinterpret_cast<const std::byte*>(serialized.data()),
                    serialized.size()));
            if (!write_res) {
                throw std::runtime_error("Failed to write block to " + seg_path.string());
            }
            writer.close();

            // Update manifest segment metadata
            auto& seg_meta = manifest.segments[col.name];
            seg_meta.rows_per_block = static_cast<uint32_t>(current_rows + 1);
            seg_meta.total_blocks = 1;
            seg_meta.poly_N = poly_N;
            seg_meta.param_scale_or_p = param_scale_or_p;
        }

        // Commit updated manifest
        manifest.total_records = current_rows + 1;
        auto ser_res = manifest.serialize(manifest_path.string());
        if (!ser_res) {
            hegadb::logging::error("[STORAGE] Failed to serialize updated manifest for '{}'", table_name);
            throw std::runtime_error("Failed to serialize manifest: " + manifest_path.string());
        }

        hegadb::logging::info("[STORAGE] Successfully appended row to table '{}' (total records now {})",
                             table_name, current_rows + 1);
        return true;
    }
};

// Factory function to create a FileStorageManager
std::unique_ptr<StorageManager> StorageManager::createFileStorage(
    std::string const& data_directory,
    std::string const& catalog_directory)
{
    return std::make_unique<FileStorageManager>(data_directory, catalog_directory);
}

} // namespace hegadb::server::storage

