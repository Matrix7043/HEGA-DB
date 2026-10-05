#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <hegadb/server/storage/table_manifest.h>
#include <hegadb/server/storage/segment_reader.h>
#include <hegadb/crypto/OpenFHECiphertext.h>

#include <filesystem>
#include <memory>
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
                    throw std::runtime_error(
                        "Inconsistent block counts across segments in table: " + 
                        manifest_.table_name
                    );
                }
            }
        }
        
        std::string const& getName() const override {
            return manifest_.table_name;
        }
        
        std::vector<std::string> getColumnNames() const override {
            std::vector<std::string> names;
            names.reserve(manifest_.columns.size());
            for (auto const& col : manifest_.columns) {
                names.push_back(col.name);
            }
            return names;
        }
        
        void open() override {
            if (is_open_) {
                return;
            }
            
            // Open segment readers for each column
            for (auto const& col : manifest_.columns) {
                auto seg_it = manifest_.segments.find(col.name);
                if (seg_it == manifest_.segments.end()) {
                    throw std::runtime_error(
                        "No segment found for column: " + col.name
                    );
                }
                
                auto segment_path = table_directory_ / seg_it->second.file;
                auto reader = std::make_unique<SegmentReader>(segment_path.string());
                
                // Read and validate header
                auto header_result = reader->read_header();
                if (!header_result.has_value()) {
                    throw std::runtime_error(
                        "Failed to read segment header for column: " + col.name
                    );
                }
                
                segment_readers_[col.name] = std::move(reader);
            }
            
            current_block_ = 0;
            is_open_ = true;
        }
        
        std::optional<EncryptedBatch> nextBatch() override {
            if (!is_open_) {
                throw std::runtime_error("Table not opened: " + manifest_.table_name);
            }
            
            // Check if we've read all blocks
            if (current_block_ >= total_blocks_) {
                return std::nullopt;
            }
            
            // Read one block from each column's segment
            EncryptedBatch batch;
            std::vector<std::byte> payload_buffer;
            
            for (auto const& col : manifest_.columns) {
                auto& reader = segment_readers_.at(col.name);
                
                payload_buffer.clear();
                auto block_header_result = reader->read_next_block(payload_buffer);
                
                if (!block_header_result.has_value()) {
                    throw std::runtime_error(
                        "Failed to read block from column: " + col.name
                    );
                }
                
                auto const& block_header = *block_header_result;
                
                // Set batch row count from first column
                if (batch.row_count == 0) {
                    batch.row_count = block_header.active_rows;
                } else if (batch.row_count != block_header.active_rows) {
                    throw std::runtime_error(
                        "Mismatched row counts across columns in block " + 
                        std::to_string(current_block_)
                    );
                }
                
                // Deserialize the ciphertext from payload
                std::string payload_str(
                    reinterpret_cast<char const*>(payload_buffer.data()),
                    payload_buffer.size()
                );
                
                try {
                    // Deserialize OpenFHE ciphertext directly (no HGCT wrapper)
                    std::istringstream blob_stream(payload_str, std::ios::in | std::ios::binary);
                    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> openfhe_ct;
                    lbcrypto::Serial::Deserialize(openfhe_ct, blob_stream, lbcrypto::SerType::BINARY);
                    
                    if (!openfhe_ct) {
                        throw std::runtime_error("Deserialized ciphertext is null");
                    }
                    
                    // Determine scheme type and data type from column metadata
                    crypto::SchemeType scheme_type = (col.scheme == "CKKS") ? 
                        crypto::SchemeType::CKKS : crypto::SchemeType::BFV;
                    
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
                        meta_length
                    );
                    
                    batch.columns.emplace(col.name, std::move(ciphertext));
                } catch (std::exception const& e) {
                    throw std::runtime_error(
                        "Failed to deserialize ciphertext for column " + 
                        col.name + ": " + e.what()
                    );
                }
            }
            
            current_block_++;
            return batch;
        }
        
        void close() override {
            if (!is_open_) {
                return;
            }
            
            segment_readers_.clear();
            current_block_ = 0;
            is_open_ = false;
        }
        
        size_t getRowCount() const override {
            return manifest_.total_records;
        }
    };
    
public:
    FileStorageManager(std::string data_directory, std::string catalog_directory)
        : data_directory_(std::move(data_directory))
        , catalog_directory_(std::move(catalog_directory))
    {
        if (!std::filesystem::exists(data_directory_)) {
            throw std::runtime_error(
                "Storage directory does not exist: " +
                data_directory_.string()
            );
        }

        if (!std::filesystem::is_directory(data_directory_)) {
            throw std::runtime_error(
                "Storage path is not a directory: " +
                data_directory_.string()
            );
        }

        if (!std::filesystem::exists(catalog_directory_)) {
            throw std::runtime_error(
                "Catalog directory does not exist: " +
                catalog_directory_.string()
            );
        }
    }
    
    std::unique_ptr<EncryptedTable> openTable(
        std::string const& table_name) override
    {
        // Construct path to table directory
        auto table_dir = data_directory_ / table_name;
        
        if (!std::filesystem::exists(table_dir)) {
            throw std::runtime_error(
                "Table directory does not exist: " + table_dir.string()
            );
        }
        
        // Load table manifest from the catalog directory
        auto manifest_path = catalog_directory_ / (table_name + ".manifest.json");
        auto manifest_result = TableManifest::deserialize(manifest_path.string());

        if (!manifest_result.has_value()) {
            throw std::runtime_error(
                "Failed to load table manifest: " + manifest_path.string()
            );
        }
        
        auto manifest = std::move(*manifest_result);
        
        // Verify table name matches
        if (manifest.table_name != table_name) {
            throw std::runtime_error(
                "Table name mismatch: expected " + table_name + 
                ", got " + manifest.table_name
            );
        }
        
        // Create and return table implementation
        return std::make_unique<SegmentBasedTable>(
            std::move(manifest),
            table_dir
        );
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
