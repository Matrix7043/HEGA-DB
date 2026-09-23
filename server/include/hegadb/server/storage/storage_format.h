#pragma once

#include <cstdint>
#include <cstddef>

namespace hegadb::server::storage {

// Magic identifiers
inline constexpr char BFV_MAGIC[8]  = {'B', 'F', 'V', '_', 'S', 'E', 'G', '\0'};
inline constexpr char CKKS_MAGIC[8] = {'C', 'K', 'K', '_', 'S', 'E', 'G', '\0'};

// 64-byte aligned file header at offset 0
struct alignas(64) SegmentFileHeader {
    char     magic_bytes[8];        // Offset 0
    uint32_t version;               // Offset 8
    uint32_t column_count;          // Offset 12
    uint32_t poly_degree_N;         // Offset 16
    uint32_t modulus_count_L;       // Offset 20
    uint32_t rows_per_block;        // Offset 24
    uint32_t reserved_32;           // Offset 28 (Explicit padding for 64-bit boundary alignment)
    uint64_t param_scale_or_p;      // Offset 32
    uint64_t total_blocks;          // Offset 40
    uint64_t total_row_count;       // Offset 48
    uint8_t  padding[8];            // Offset 56 (Padding to exactly 64 bytes)
};
static_assert(sizeof(SegmentFileHeader) == 64, "SegmentFileHeader must be exactly 64 bytes");

// 64-byte aligned block header preceding each ciphertext CRT payload
struct alignas(64) BlockHeader {
    uint64_t block_id;              // Offset 0
    uint32_t active_rows;           // Offset 8
    uint32_t current_noise_budget;  // Offset 12
    uint64_t payload_byte_size;     // Offset 16
    uint32_t crc32_checksum;        // Offset 24
    uint8_t  padding[36];           // Offset 28 (Padding to exactly 64 bytes)
};
static_assert(sizeof(BlockHeader) == 64, "BlockHeader must be exactly 64 bytes");

} // namespace hegadb::server::storage
