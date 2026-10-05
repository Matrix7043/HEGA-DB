#ifndef SEGMENT_READER_H
#define SEGMENT_READER_H

#include "io_utils.h"
#include "storage_format.h"
#include <span>
#include <string>
#include <vector>

namespace hegadb::server::storage {

class SegmentReader {
public:
    explicit SegmentReader(std::string const& file_path);
    ~SegmentReader() = default;

    ValueResult<SegmentFileHeader> read_header();

    // Reads the next block into the provided buffer.
    // Buffer should be large enough, or use vector allocation.
    ValueResult<BlockHeader> read_next_block(std::vector<std::byte>& payload_buffer);

private:
    std::string file_path_;
    UniqueFd fd_;
    uint64_t current_offset_ { 0 };
    SegmentFileHeader header_;

    uint32_t calculate_crc32(BlockHeader const& header, std::span<std::byte const> payload) const;
};

} // namespace hegadb::server::storage
#endif
