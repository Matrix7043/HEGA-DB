#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <expected>
#include "io_utils.h"

namespace hegadb::server::storage {

struct ColumnMetadata {
    std::string name;
    std::string type;
    std::string scheme;
    uint32_t slot_index;
};

struct SegmentMetadata {
    std::string file;
    uint32_t poly_N;
    uint64_t param_scale_or_p;
    uint32_t rows_per_block;
    uint64_t total_blocks;
};

struct TableManifest {
    std::string table_name;
    uint64_t total_records{0};
    std::unordered_map<std::string, uint64_t> dictionary;
    std::vector<ColumnMetadata> columns;
    std::unordered_map<std::string, SegmentMetadata> segments;

    // Atomic write-replace (writes to .tmp, fsyncs, then renames to target)
    StorageResult serialize(const std::string& file_path) const;
    
    // Reads from standard JSON-like format
    static ValueResult<TableManifest> deserialize(const std::string& file_path);
};

} // namespace hegadb::server::storage
