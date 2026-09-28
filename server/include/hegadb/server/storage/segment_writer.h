#pragma once

#include "storage_format.h"
#include "io_utils.h"
#include <span>
#include <string>
#include <expected>

namespace hegadb::server::storage {

class SegmentWriter {
public:
    SegmentWriter(const std::string& file_path, const SegmentFileHeader& header);
    ~SegmentWriter() = default;

    // Writes a block header and its payload to disk.
    // Handles 512-byte padding for future O_DIRECT compatibility if ever enabled.
    StorageResult write_block(
        const BlockHeader& header,
        std::span<const std::byte> payload);

    StorageResult close();

private:
    std::string file_path_;
    UniqueFd fd_;
    SegmentFileHeader header_;
    uint64_t current_offset_{0};

    uint32_t calculate_crc32(const BlockHeader& header, std::span<const std::byte> payload) const;
};

} // namespace hegadb::server::storage
