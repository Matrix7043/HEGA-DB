#ifndef SEGMENT_WRITER_H
#define SEGMENT_WRITER_H

#include "IOUtils.h"
#include "StorageFormat.h"
#include <span>
#include <string>

namespace hegadb::server::storage {

class SegmentWriter {
public:
    SegmentWriter(std::string const& file_path, SegmentFileHeader const& header);
    ~SegmentWriter() = default;

    // Writes a block header and its payload to disk.
    // Handles 512-byte padding for future O_DIRECT compatibility if ever enabled.
    StorageResult write_block(
        BlockHeader const& header,
        std::span<std::byte const> payload);

    StorageResult close();

private:
    std::string file_path_;
    UniqueFd fd_;
    SegmentFileHeader header_;
    uint64_t current_offset_ { 0 };

    uint32_t calculate_crc32(BlockHeader const& header, std::span<std::byte const> payload) const;
};

} // namespace hegadb::server::storage
#endif
