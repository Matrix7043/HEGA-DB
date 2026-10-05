#ifndef PINNED_MEMORY_POOL_H
#define PINNED_MEMORY_POOL_H

#include <cstddef>
#include <span>

namespace hegadb::server::storage {

class PinnedMemoryPool {
public:
    explicit PinnedMemoryPool(size_t block_capacity_bytes);
    ~PinnedMemoryPool();

    PinnedMemoryPool(PinnedMemoryPool const&) = delete;
    PinnedMemoryPool& operator=(PinnedMemoryPool const&) = delete;

    [[nodiscard]] std::span<std::byte> get_primary_buffer() noexcept;
    [[nodiscard]] std::span<std::byte> get_secondary_buffer() noexcept;
    void swap_buffers() noexcept;

private:
    size_t capacity_ { 0 };
    std::byte* primary_buf_ { nullptr };
    std::byte* secondary_buf_ { nullptr };
};

} // namespace hegadb::server::storage
#endif
