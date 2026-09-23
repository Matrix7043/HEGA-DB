#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <expected>
#include <cstdint>
#include <cstdlib>

namespace hegadb::server::storage {

enum class StorageError {
    FileNotFound,
    PermissionDenied,
    DiskFull,
    InvalidFormat,
    ChecksumMismatch,
    IOError,
    InvalidDictionaryId,
    ManifestCorruption
};

struct StorageResult {
    bool success;
    StorageError error;
    StorageResult() : success(true), error(StorageError::FileNotFound) {}
    StorageResult(StorageError err) : success(false), error(err) {}
    operator bool() const { return success; }
};

template <typename T>
struct ValueResult {
    bool success;
    T value;
    StorageError error;
    ValueResult(T val) : success(true), value(std::move(val)), error(StorageError::FileNotFound) {}
    ValueResult(StorageError err) : success(false), value(), error(err) {}
    bool has_value() const { return success; }
    T& operator*() { return value; }
    T* operator->() { return &value; }
};

// RAII File Descriptor wrapper
class UniqueFd {
public:
    UniqueFd() noexcept : fd_(-1) {}
    explicit UniqueFd(int fd) noexcept : fd_(fd) {}
    
    ~UniqueFd() {
        reset();
    }
    
    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;
    
    UniqueFd(UniqueFd&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }
    
    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (this != &other) {
            reset(other.fd_);
            other.fd_ = -1;
        }
        return *this;
    }
    
    int get() const noexcept { return fd_; }
    bool is_valid() const noexcept { return fd_ >= 0; }
    
    void reset(int new_fd = -1) noexcept {
        if (fd_ >= 0 && fd_ != new_fd) {
            ::close(fd_);
        }
        fd_ = new_fd;
    }
    
private:
    int fd_;
};

// Aligned Memory Allocator helper
inline void* aligned_allocate(size_t alignment, size_t size) {
    void* ptr = nullptr;
    if (::posix_memalign(&ptr, alignment, size) != 0) {
        throw std::bad_alloc();
    }
    return ptr;
}

inline void aligned_deallocate(void* ptr) noexcept {
    ::free(ptr);
}

} // namespace hegadb::server::storage
