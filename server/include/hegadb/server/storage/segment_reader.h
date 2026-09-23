#pragma once

#include "storage_format.h"
#include "io_utils.h"
#include <span>
#include <string>
#include <expected>
#include <vector>

namespace hegadb::server::storage {

class SegmentReader {
public:
    explicit SegmentReader(const std::string& file_path);
    ~SegmentReader() = default;

    ValueResult<SegmentFileHeader> read_header();
    
    // Reads the next block into the provided buffer.
    // Buffer should be large enough, or use vector allocation.
    ValueResult<BlockHeader> read_next_block(std::vector<std::byte>& payload_buffer);

private:
    std::string file_path_;
    UniqueFd fd_;
    uint64_t current_offset_{0};
    SegmentFileHeader header_;

    uint32_t calculate_crc32(const BlockHeader& header, std::span<const std::byte> payload) const;
};

} // namespace hegadb::server::storage
