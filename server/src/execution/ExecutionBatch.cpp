#include <hegadb/server/execution/ExecutionBatch.h>
#include <stdexcept>

namespace hegadb::server::execution {

void ExecutionBatch::setRowCount(size_t count) {
    row_count_ = count;
}

size_t ExecutionBatch::getRowCount() const {
    return row_count_;
}

void ExecutionBatch::addColumn(
    std::string const& column_name,
    crypto::OpenFHECiphertext ciphertext) {

    auto [it, inserted] = columns_.emplace(column_name, std::move(ciphertext));
    if (!inserted) {
        throw std::runtime_error("Column already exists: " + column_name);
    }
}

crypto::OpenFHECiphertext const& ExecutionBatch::getColumn(
    std::string const& column_name) const {

    auto it = columns_.find(column_name);
    if (it == columns_.end()) {
        throw std::out_of_range("Column not found: " + column_name);
    }
    return it->second;
}

bool ExecutionBatch::hasColumn(std::string const& column_name) const {
    return columns_.find(column_name) != columns_.end();
}

std::vector<std::string> ExecutionBatch::getColumnNames() const {
    std::vector<std::string> names;
    names.reserve(columns_.size());
    for (auto const& [name, _] : columns_) {
        names.push_back(name);
    }
    return names;
}

bool ExecutionBatch::empty() const {
    return row_count_ == 0 || columns_.empty();
}

size_t ExecutionBatch::getColumnCount() const {
    return columns_.size();
}

bool ExecutionBatch::removeColumn(std::string const& column_name) {
    return columns_.erase(column_name) > 0;
}

void ExecutionBatch::clear() {
    row_count_ = 0;
    columns_.clear();
}

} // namespace hegadb::server::execution
