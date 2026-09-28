#include "hegadb/server/storage/pinned_memory_pool.h"
#include "hegadb/server/storage/io_utils.h"

#ifdef HEGADB_ENABLE_CUDA
#    include <cuda_runtime.h>
#    include <stdexcept>
#endif

namespace hegadb::server::storage {

PinnedMemoryPool::PinnedMemoryPool(size_t block_capacity_bytes)
    : capacity_(block_capacity_bytes)
{
#ifdef HEGADB_ENABLE_CUDA
    cudaError_t err = cudaMallocHost(reinterpret_cast<void**>(&primary_buf_), capacity_);
    if (err != cudaSuccess) {
        throw std::runtime_error("cudaMallocHost failed for primary_buf_");
    }
    err = cudaMallocHost(reinterpret_cast<void**>(&secondary_buf_), capacity_);
    if (err != cudaSuccess) {
        cudaFreeHost(primary_buf_);
        throw std::runtime_error("cudaMallocHost failed for secondary_buf_");
    }
#else
    primary_buf_ = static_cast<std::byte*>(aligned_allocate(64, capacity_));
    secondary_buf_ = static_cast<std::byte*>(aligned_allocate(64, capacity_));
#endif
}

PinnedMemoryPool::~PinnedMemoryPool()
{
#ifdef HEGADB_ENABLE_CUDA
    if (primary_buf_)
        cudaFreeHost(primary_buf_);
    if (secondary_buf_)
        cudaFreeHost(secondary_buf_);
#else
    if (primary_buf_)
        aligned_deallocate(primary_buf_);
    if (secondary_buf_)
        aligned_deallocate(secondary_buf_);
#endif
}

std::span<std::byte> PinnedMemoryPool::get_primary_buffer() noexcept
{
    return std::span<std::byte>(primary_buf_, capacity_);
}

std::span<std::byte> PinnedMemoryPool::get_secondary_buffer() noexcept
{
    return std::span<std::byte>(secondary_buf_, capacity_);
}

void PinnedMemoryPool::swap_buffers() noexcept
{
    std::byte* temp = primary_buf_;
    primary_buf_ = secondary_buf_;
    secondary_buf_ = temp;
}

} // namespace hegadb::server::storage
