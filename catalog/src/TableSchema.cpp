#include "hegadb/catalog/TableSchema.h"
#include <utility>

namespace hegadb::catalog {

TableSchema::TableSchema(std::string name, std::vector<ColumnSchema> columns)
    : name_(std::move(name))
    , columns_(std::move(columns))
{
}

std::string const& TableSchema::name() const { return name_; }

std::vector<ColumnSchema> const& TableSchema::columns() const
{
    return columns_;
}

ColumnSchema const* TableSchema::findColumn(std::string const& name) const
{

    for (auto const& column : columns_) {
        if (column.name == name)
            return &column;
    }

    return nullptr;
}

} // namespace hegadb
