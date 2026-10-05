#include "hegadb/server/storage/segment_writer.h"
#include <hegadb/logging/Logger.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <vector>

namespace hegadb::server::storage {

// Simple CRC32 for block integrity
uint32_t SegmentWriter::calculate_crc32(BlockHeader const& header, std::span<std::byte const> payload) const
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

    // Don't include the crc field itself in the calculation
    process_bytes(reinterpret_cast<std::byte const*>(&header), offsetof(BlockHeader, crc32_checksum));
    process_bytes(payload.data(), payload.size());

    return ~crc;
}

SegmentWriter::SegmentWriter(std::string const& file_path, SegmentFileHeader const& header)
    : file_path_(file_path)
    , header_(header)
{

    // Use standard POSIX open without O_DIRECT for now as requested by user
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = ::open(file_path.c_str(), flags, mode);
    if (fd < 0) {
        hegadb::logging::error("Failed to open segment file for writing: '{}' (errno={})", file_path, errno);
        throw std::runtime_error("Failed to open segment file for writing: " + file_path);
    }
    fd_.reset(fd);

    // Write the 64-byte file header
    ssize_t written = ::pwrite(fd_.get(), &header_, sizeof(header_), 0);
    if (written != sizeof(header_)) {
        hegadb::logging::error("Failed to write segment file header to '{}'", file_path);
        throw std::runtime_error("Failed to write segment file header");
    }

    current_offset_ = sizeof(header_);
    hegadb::logging::info("Initialized segment writer for file: '{}'", file_path_);
}

StorageResult SegmentWriter::write_block(
    BlockHeader const& header,
    std::span<std::byte const> payload)
{

    if (!fd_.is_valid()) {
        hegadb::logging::error("Attempted write_block on invalid file descriptor for file: '{}'", file_path_);
        return StorageError::IOError;
    }

    BlockHeader final_header = header;
    final_header.payload_byte_size = payload.size();
    final_header.crc32_checksum = calculate_crc32(final_header, payload);

    // Write header
    ssize_t written = ::pwrite(fd_.get(), &final_header, sizeof(final_header), current_offset_);
    if (written != sizeof(final_header)) {
        hegadb::logging::error("Failed to write block header to '{}' at offset {}", file_path_, current_offset_);
        return StorageError::IOError;
    }
    current_offset_ += written;

    // Write payload
    if (!payload.empty()) {
        written = ::pwrite(fd_.get(), payload.data(), payload.size(), current_offset_);
        if (written != static_cast<ssize_t>(payload.size())) {
            hegadb::logging::error("Failed to write payload to '{}' at offset {}", file_path_, current_offset_);
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
            hegadb::logging::error("Failed to write alignment padding to '{}' at offset {}", file_path_, current_offset_);
            return StorageError::IOError;
        }
        current_offset_ += pad_size;
    }

    hegadb::logging::trace("Wrote block to '{}': payload_size={}, crc32=0x{:08x}, new_offset={}",
                           file_path_, payload.size(), final_header.crc32_checksum, current_offset_);

    return {};
}

StorageResult SegmentWriter::close()
{
    if (fd_.is_valid()) {
        if (::fsync(fd_.get()) != 0) {
            hegadb::logging::error("fsync failed during close on segment file '{}' (errno={})", file_path_, errno);
            return StorageError::IOError;
        }
        hegadb::logging::info("Closed segment writer for '{}' (total bytes: {})", file_path_, current_offset_);
        fd_.reset();
    }
    return {};
}

} // namespace hegadb::server::storage
