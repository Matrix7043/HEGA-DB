#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <expected>
#include <hegadb/server/storage/io_utils.h>

namespace hegadb::server::storage {

class DictionaryEncoder {
public:
    DictionaryEncoder() = default;

    // Returns a 1-indexed ID. ID 0 is strictly reserved.
    uint64_t encode(const std::string& value);

    // Look up ID. Returns InvalidDictionaryId if id is 0 or unknown.
    ValueResult<std::string> decode(uint64_t id) const;

    const std::unordered_map<std::string, uint64_t>& get_mapping() const {
        return mapping_;
    }

    void load_mapping(const std::unordered_map<std::string, uint64_t>& mapping);

private:
    std::unordered_map<std::string, uint64_t> mapping_;
    std::unordered_map<uint64_t, std::string> reverse_mapping_;
    uint64_t next_id_{1}; // 0 is reserved
};

} // namespace hegadb::server::storage
