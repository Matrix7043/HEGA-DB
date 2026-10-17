#include <hegadb/tools/setup/FileStorageWriter.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <iomanip>

namespace hegadb::tools::setup {

FileStorageWriter::FileStorageWriter(std::string base_dir)
    : base_dir_(std::move(base_dir))
{
    ensureDirectoryExists(base_dir_);
}

void FileStorageWriter::writeTable(
    std::string const& table_name,
    std::vector<std::string> const& column_names,
    std::vector<types::DataType> const& column_types,
    std::vector<crypto::OpenFHECiphertext> const& encrypted_columns,
    size_t row_count)
{
    if (column_names.size() != column_types.size() ||
        column_names.size() != encrypted_columns.size()) {
        throw std::runtime_error("Column metadata size mismatch");
    }

    // Create table directory
    std::string table_path = getTablePath(table_name);
    ensureDirectoryExists(table_path);

    std::cout << "[WRITER]  Writing table: " << table_name << "\n";
    std::cout << "[WRITER]  Location: " << table_path << "\n";

    // Write each column as a segment file
    for (size_t i = 0; i < column_names.size(); ++i) {
        std::string segment_file = table_path + "/" + column_names[i] + ".seg";
        
        std::cout << "[WRITER]    Column[" << i << "]: " << column_names[i] 
                  << " -> " << segment_file << "\n";

        writeColumnSegment(
            column_names[i],
            column_types[i],
            encrypted_columns[i],
            row_count,
            segment_file
        );
    }

    std::cout << "[WRITER]  ✓ Table written successfully\n";
}

void FileStorageWriter::writeColumnSegment(
    std::string const& column_name,
    types::DataType column_type,
    crypto::OpenFHECiphertext const& ciphertext,
    size_t row_count,
    std::string const& output_path)
{
    // Get the OpenFHE ciphertext
    auto const& openfhe_ct = ciphertext.getCiphertext();

    // Determine scheme type
    bool is_bfv = (ciphertext.getSchemeType() == crypto::SchemeType::BFV);
    
    // Get crypto context parameters
    auto cc = openfhe_ct->GetCryptoContext();
    auto elem_params = cc->GetElementParams();
    uint32_t poly_N = elem_params->GetRingDimension();
    uint32_t modulus_count = elem_params->GetParams().size();
    
    // For BFV, get plaintext modulus; for CKKS, get scale
    uint64_t param_scale_or_p = is_bfv ? 
        cc->GetCryptoParameters()->GetPlaintextModulus() :
        static_cast<uint64_t>(openfhe_ct->GetScalingFactor());
    
    // Create segment file header
    server::storage::SegmentFileHeader seg_header{};
    
    // Set magic bytes based on scheme
    if (is_bfv) {
        std::memcpy(seg_header.magic_bytes, server::storage::BFV_MAGIC, 8);
    } else {
        std::memcpy(seg_header.magic_bytes, server::storage::CKKS_MAGIC, 8);
    }
    
    seg_header.version = 1;
    seg_header.column_count = 1;  // One column per segment
    seg_header.poly_degree_N = poly_N;
    seg_header.modulus_count_L = modulus_count;
    seg_header.rows_per_block = static_cast<uint32_t>(row_count);  // All rows in one block
    seg_header.param_scale_or_p = param_scale_or_p;
    seg_header.total_blocks = 1;  // Single block for simplicity
    seg_header.total_row_count = row_count;
    seg_header.reserved_32 = 0;
    std::memset(seg_header.padding, 0, sizeof(seg_header.padding));
    
    // Create segment writer
    server::storage::SegmentWriter writer(output_path, seg_header);
    
    // Serialize the ciphertext
    std::stringstream ss;
    lbcrypto::Serial::Serialize(openfhe_ct, ss, lbcrypto::SerType::BINARY);
    std::string serialized = ss.str();
    
    // Create block header
    server::storage::BlockHeader block_header{};
    block_header.block_id = 0;
    block_header.active_rows = static_cast<uint32_t>(row_count);
    block_header.current_noise_budget = 0;  // Would need actual measurement
    block_header.payload_byte_size = serialized.size();
    block_header.crc32_checksum = 0;  // Will be calculated by SegmentWriter
    std::memset(block_header.padding, 0, sizeof(block_header.padding));
    
    // Write the block
    auto result = writer.write_block(
        block_header,
        std::span<const std::byte>(
            reinterpret_cast<const std::byte*>(serialized.data()),
            serialized.size()
        )
    );
    
    if (!result) {
        throw std::runtime_error("Failed to write segment block: " + output_path);
    }
    
    // Close the segment file
    result = writer.close();
    if (!result) {
        throw std::runtime_error("Failed to close segment file: " + output_path);
    }
}

void FileStorageWriter::writeManifest(
    catalog::TableSchema const& schema,
    std::string const& catalog_dir,
    size_t row_count)
{
    ensureDirectoryExists(catalog_dir);

    // Create TableManifest using the server's storage format
    server::storage::TableManifest manifest;
    manifest.table_name = schema.name();
    manifest.total_records = row_count;
    
    // Build column metadata
    for (auto const& col : schema.columns()) {
        server::storage::ColumnMetadata col_meta;
        col_meta.name = col.name;
        
        // Convert type to string
        switch (col.type) {
            case types::DataType::Integer: col_meta.type = "INTEGER"; break;
            case types::DataType::String: col_meta.type = "STRING"; break;
            case types::DataType::Boolean: col_meta.type = "BOOLEAN"; break;
            case types::DataType::Float: col_meta.type = "FLOAT"; break;
            default: col_meta.type = "UNKNOWN"; break;
        }
        
        // Determine scheme (BFV for integers/strings, CKKS for floats)
        col_meta.scheme = (col.type == types::DataType::Float) ? "CKKS" : "BFV";
        col_meta.slot_index = static_cast<uint32_t>(col.ordinal);
        
        manifest.columns.push_back(col_meta);
        
        // Add segment metadata for this column
        server::storage::SegmentMetadata seg_meta;
        seg_meta.file = col.name + ".seg";
        seg_meta.poly_N = 16384;  // TODO: Get from actual crypto params
        seg_meta.param_scale_or_p = (col.type == types::DataType::Float) ? 
            (1ULL << 40) : 65537;  // TODO: Get from actual crypto params
        seg_meta.rows_per_block = static_cast<uint32_t>(row_count);
        seg_meta.total_blocks = 1;
        
        manifest.segments[col.name] = seg_meta;
    }
    
    // Serialize the manifest
    std::string manifest_file = catalog_dir + "/" + schema.name() + ".manifest.json";
    auto result = manifest.serialize(manifest_file);
    
    if (!result) {
        throw std::runtime_error("Failed to write manifest: " + manifest_file);
    }
    
    std::cout << "[WRITER]  ✓ Manifest written: " << manifest_file << "\n";
}

std::string FileStorageWriter::getTablePath(std::string const& table_name) const {
    return base_dir_ + "/" + table_name;
}

void FileStorageWriter::ensureDirectoryExists(std::string const& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return; // Directory exists
        } else {
            throw std::runtime_error("Path exists but is not a directory: " + path);
        }
    }

    // Directory doesn't exist, create it
    std::string command = "mkdir -p " + path;
    int result = system(command.c_str());
    
    if (result != 0) {
        throw std::runtime_error("Failed to create directory: " + path);
    }
}

} // namespace hegadb::tools::setup
