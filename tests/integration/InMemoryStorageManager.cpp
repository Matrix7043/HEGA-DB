#include "InMemoryStorageManager.h"
#include <stdexcept>

namespace hegadb::test {

// ============================================================
// InMemoryTable Implementation
// ============================================================

InMemoryTable::InMemoryTable(InMemoryTableData data)
    : data_(std::move(data))
{
}

std::string const& InMemoryTable::getName() const {
    return data_.table_name;
}

std::vector<std::string> InMemoryTable::getColumnNames() const {
    return data_.column_names;
}

void InMemoryTable::open() {
    is_open_ = true;
    batch_returned_ = false;
}

std::optional<server::storage::EncryptedBatch> InMemoryTable::nextBatch() {
    if (!is_open_) {
        throw std::runtime_error("Table not opened: " + data_.table_name);
    }

    // Return single batch containing all data
    if (batch_returned_) {
        return std::nullopt;
    }

    server::storage::EncryptedBatch batch;
    batch.row_count = data_.row_count;

    // Add each column's ciphertext to the batch
    for (size_t i = 0; i < data_.column_names.size(); ++i) {
        batch.columns.emplace(data_.column_names[i], data_.columns[i]);
    }

    batch_returned_ = true;
    return batch;
}

void InMemoryTable::close() {
    is_open_ = false;
    batch_returned_ = false;
}

size_t InMemoryTable::getRowCount() const {
    return data_.row_count;
}

// ============================================================
// InMemoryStorageManager Implementation
// ============================================================

void InMemoryStorageManager::addTable(
    std::string const& table_name,
    std::vector<std::string> const& column_names,
    std::vector<crypto::OpenFHECiphertext> const& columns,
    size_t row_count)
{
    InMemoryTableData data;
    data.table_name = table_name;
    data.column_names = column_names;
    data.columns = columns;
    data.row_count = row_count;

    tables_[table_name] = std::move(data);
}

std::unique_ptr<server::storage::EncryptedTable> InMemoryStorageManager::openTable(
    std::string const& table_name)
{
    auto it = tables_.find(table_name);
    if (it == tables_.end()) {
        throw std::runtime_error("Table not found in memory: " + table_name);
    }

    return std::make_unique<InMemoryTable>(it->second);
}

bool InMemoryStorageManager::hasTable(std::string const& table_name) const {
    return tables_.find(table_name) != tables_.end();
}

void InMemoryStorageManager::clear() {
    tables_.clear();
}

} // namespace hegadb::test
