#include "dictionary_encoder.h"

namespace hegadb::server::storage {

uint64_t DictionaryEncoder::encode(const std::string& value) {
    auto it = mapping_.find(value);
    if (it != mapping_.end()) {
        return it->second;
    }
    uint64_t new_id = next_id_++;
    mapping_[value] = new_id;
    reverse_mapping_[new_id] = value;
    return new_id;
}

ValueResult<std::string> DictionaryEncoder::decode(uint64_t id) const {
    if (id == 0) {
        return StorageError::InvalidDictionaryId;
    }
    auto it = reverse_mapping_.find(id);
    if (it != reverse_mapping_.end()) {
        return it->second;
    }
    return StorageError::InvalidDictionaryId;
}

void DictionaryEncoder::load_mapping(const std::unordered_map<std::string, uint64_t>& mapping) {
    mapping_ = mapping;
    reverse_mapping_.clear();
    uint64_t max_id = 0;
    for (const auto& [str, id] : mapping) {
        reverse_mapping_[id] = str;
        if (id > max_id) {
            max_id = id;
        }
    }
    next_id_ = max_id + 1;
}

} // namespace hegadb::server::storage
