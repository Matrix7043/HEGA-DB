#include "catalog/TableSchema.h"
#include <utility>

namespace hegadb {

TableSchema::TableSchema(std::string name, std::vector<ColumnSchema> columns)
    : name_(std::move(name)), columns_(std::move(columns)) {}

const std::string &TableSchema::name() const { return name_; }

const std::vector<ColumnSchema> &TableSchema::columns() const {
  return columns_;
}

const ColumnSchema *TableSchema::findColumn(const std::string &name) const {

  for (auto const &column : columns_) {
    if (column.name == name)
      return &column;
  }

  return nullptr;
}

} // namespace hegadb
