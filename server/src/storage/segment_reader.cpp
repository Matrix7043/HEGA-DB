#include "hegadb/server/storage/segment_reader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace hegadb::server::storage {

uint32_t SegmentReader::calculate_crc32(const BlockHeader& header, std::span<const std::byte> payload) const {
    uint32_t crc = 0xFFFFFFFF;
    
    auto process_bytes = [&crc](const std::byte* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
        }
    };
    
    process_bytes(reinterpret_cast<const std::byte*>(&header), offsetof(BlockHeader, crc32_checksum));
    process_bytes(payload.data(), payload.size());
    
    return ~crc;
}

SegmentReader::SegmentReader(const std::string& file_path)
    : file_path_(file_path) {
    int fd = ::open(file_path.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Failed to open segment file for reading: " + file_path);
    }
    fd_.reset(fd);
}

ValueResult<SegmentFileHeader> SegmentReader::read_header() {
    ssize_t bytes_read = ::pread(fd_.get(), &header_, sizeof(header_), 0);
    if (bytes_read != sizeof(header_)) {
        return StorageError::IOError;
    }
    current_offset_ = sizeof(header_);
    return header_;
}

ValueResult<BlockHeader> SegmentReader::read_next_block(std::vector<std::byte>& payload_buffer) {
    BlockHeader block_header;
    ssize_t bytes_read = ::pread(fd_.get(), &block_header, sizeof(block_header), current_offset_);
    if (bytes_read == 0) {
        // EOF
        return StorageError::FileNotFound; // Reusing as EOF indicator
    }
    if (bytes_read != sizeof(block_header)) {
        return StorageError::IOError;
    }
    
    current_offset_ += sizeof(block_header);
    
    payload_buffer.resize(block_header.payload_byte_size);
    if (!payload_buffer.empty()) {
        bytes_read = ::pread(fd_.get(), payload_buffer.data(), payload_buffer.size(), current_offset_);
        if (bytes_read != static_cast<ssize_t>(payload_buffer.size())) {
            return StorageError::IOError;
        }
        current_offset_ += bytes_read;
    }
    
    // Check CRC
    uint32_t calculated_crc = calculate_crc32(block_header, payload_buffer);
    if (calculated_crc != block_header.crc32_checksum) {
        return StorageError::ChecksumMismatch;
    }
    
    // Skip padding to 512 bytes boundary
    uint64_t unaligned = current_offset_ % 512;
    if (unaligned != 0) {
        current_offset_ += (512 - unaligned);
    }

    return block_header;
}

} // namespace hegadb::server::storage
