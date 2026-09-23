#pragma once

#include <cstddef>
#include <span>

namespace hegadb::server::storage {

class PinnedMemoryPool {
public:
    explicit PinnedMemoryPool(size_t block_capacity_bytes);
    ~PinnedMemoryPool();

    PinnedMemoryPool(const PinnedMemoryPool&) = delete;
    PinnedMemoryPool& operator=(const PinnedMemoryPool&) = delete;

    [[nodiscard]] std::span<std::byte> get_primary_buffer() noexcept;
    [[nodiscard]] std::span<std::byte> get_secondary_buffer() noexcept;
    void swap_buffers() noexcept;

private:
    size_t capacity_{0};
    std::byte* primary_buf_{nullptr};
    std::byte* secondary_buf_{nullptr};
};

} // namespace hegadb::server::storage
