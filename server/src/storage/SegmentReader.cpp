#include "hegadb/server/storage/SegmentReader.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <hegadb/logging/Logger.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace hegadb::server::storage {

uint32_t SegmentReader::calculate_crc32(BlockHeader const& header, std::span<std::byte const> payload) const
{
    uint32_t crc = 0xFFFFFFFF;

    auto process_bytes = [&crc](std::byte const* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
        }
    };

    process_bytes(reinterpret_cast<std::byte const*>(&header), offsetof(BlockHeader, crc32_checksum));
    process_bytes(payload.data(), payload.size());

    return ~crc;
}

SegmentReader::SegmentReader(std::string const& file_path)
    : file_path_(file_path)
{
    int fd = ::open(file_path.c_str(), O_RDONLY);
    if (fd < 0) {
        hegadb::logging::error("Failed to open segment file for reading: '{}' (errno={})", file_path, errno);
        throw std::runtime_error("Failed to open segment file for reading: " + file_path);
    }
    fd_.reset(fd);
    hegadb::logging::debug("Opened segment reader for file: '{}'", file_path_);
}

ValueResult<SegmentFileHeader> SegmentReader::read_header()
{
    ssize_t bytes_read = ::pread(fd_.get(), &header_, sizeof(header_), 0);
    if (bytes_read != sizeof(header_)) {
        hegadb::logging::error("Failed to read segment header from '{}': expected {} bytes, got {}",
            file_path_, sizeof(header_), bytes_read);
        return StorageError::IOError;
    }
    current_offset_ = sizeof(header_);
    hegadb::logging::debug("Successfully read segment header from '{}' (magic: {:.8s}, version: {})",
        file_path_, header_.magic_bytes, header_.version);
    return header_;
}

ValueResult<BlockHeader> SegmentReader::read_next_block(std::vector<std::byte>& payload_buffer)
{
    BlockHeader block_header;
    ssize_t bytes_read = ::pread(fd_.get(), &block_header, sizeof(block_header), current_offset_);
    if (bytes_read == 0) {
        // EOF
        hegadb::logging::debug("Reached end of file for segment: '{}' at offset {}", file_path_, current_offset_);
        return StorageError::FileNotFound; // Reusing as EOF indicator
    }
    if (bytes_read != sizeof(block_header)) {
        hegadb::logging::error("Incomplete block header read in '{}' at offset {}: expected {} bytes, got {}",
            file_path_, current_offset_, sizeof(block_header), bytes_read);
        return StorageError::IOError;
    }

    current_offset_ += sizeof(block_header);

    payload_buffer.resize(block_header.payload_byte_size);
    if (!payload_buffer.empty()) {
        bytes_read = ::pread(fd_.get(), payload_buffer.data(), payload_buffer.size(), current_offset_);
        if (bytes_read != static_cast<ssize_t>(payload_buffer.size())) {
            hegadb::logging::error("Incomplete payload read in '{}' at offset {}: expected {} bytes, got {}",
                file_path_, current_offset_, payload_buffer.size(), bytes_read);
            return StorageError::IOError;
        }
        current_offset_ += bytes_read;
    }

    // Check CRC
    uint32_t calculated_crc = calculate_crc32(block_header, payload_buffer);
    if (calculated_crc != block_header.crc32_checksum) {
        hegadb::logging::error("CRC32 checksum mismatch in segment '{}' at offset {}: expected 0x{:08x}, got 0x{:08x}",
            file_path_, current_offset_ - payload_buffer.size() - sizeof(block_header),
            block_header.crc32_checksum, calculated_crc);
        return StorageError::ChecksumMismatch;
    }

    // Skip padding to 512 bytes boundary
    uint64_t unaligned = current_offset_ % 512;
    if (unaligned != 0) {
        current_offset_ += (512 - unaligned);
    }

    hegadb::logging::trace("Read block from '{}': block_id={}, payload_size={}, crc32=0x{:08x}",
        file_path_, block_header.block_id, block_header.payload_byte_size, block_header.crc32_checksum);

    return block_header;
}

} // namespace hegadb::server::storage
