#include "hegadb/server/storage/segment_writer.h"
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <vector>

namespace hegadb::server::storage {

// Simple CRC32 for block integrity
uint32_t SegmentWriter::calculate_crc32(const BlockHeader& header, std::span<const std::byte> payload) const {
    uint32_t crc = 0xFFFFFFFF;
    
    auto process_bytes = [&crc](const std::byte* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
        }
    };
    
    // Don't include the crc field itself in the calculation
    process_bytes(reinterpret_cast<const std::byte*>(&header), offsetof(BlockHeader, crc32_checksum));
    process_bytes(payload.data(), payload.size());
    
    return ~crc;
}

SegmentWriter::SegmentWriter(const std::string& file_path, const SegmentFileHeader& header)
    : file_path_(file_path), header_(header) {
    
    // Use standard POSIX open without O_DIRECT for now as requested by user
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    int fd = ::open(file_path.c_str(), flags, mode);
    if (fd < 0) {
        throw std::runtime_error("Failed to open segment file for writing: " + file_path);
    }
    fd_.reset(fd);
    
    // Write the 64-byte file header
    ssize_t written = ::pwrite(fd_.get(), &header_, sizeof(header_), 0);
    if (written != sizeof(header_)) {
        throw std::runtime_error("Failed to write segment file header");
    }
    
    current_offset_ = sizeof(header_);
}

StorageResult SegmentWriter::write_block(
    const BlockHeader& header,
    std::span<const std::byte> payload) {
    
    if (!fd_.is_valid()) {
        return StorageError::IOError;
    }

    BlockHeader final_header = header;
    final_header.payload_byte_size = payload.size();
    final_header.crc32_checksum = calculate_crc32(final_header, payload);

    // Write header
    ssize_t written = ::pwrite(fd_.get(), &final_header, sizeof(final_header), current_offset_);
    if (written != sizeof(final_header)) {
        return StorageError::IOError;
    }
    current_offset_ += written;

    // Write payload
    if (!payload.empty()) {
        written = ::pwrite(fd_.get(), payload.data(), payload.size(), current_offset_);
        if (written != static_cast<ssize_t>(payload.size())) {
            return StorageError::IOError;
        }
        current_offset_ += written;
    }

    // Explicit 512-byte padding for O_DIRECT future compatibility if we want it.
    // We pad the file so the next block starts at a 512-byte boundary relative to 0.
    uint64_t unaligned = current_offset_ % 512;
    if (unaligned != 0) {
        uint64_t pad_size = 512 - unaligned;
        std::vector<char> pad(pad_size, 0);
        written = ::pwrite(fd_.get(), pad.data(), pad.size(), current_offset_);
        if (written != static_cast<ssize_t>(pad_size)) {
            return StorageError::IOError;
        }
        current_offset_ += pad_size;
    }

    return {};
}

StorageResult SegmentWriter::close() {
    if (fd_.is_valid()) {
        if (::fsync(fd_.get()) != 0) {
            return StorageError::IOError;
        }
        fd_.reset();
    }
    return {};
}

} // namespace hegadb::server::storage
