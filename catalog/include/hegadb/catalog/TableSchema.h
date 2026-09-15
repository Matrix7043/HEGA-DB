#ifndef TABLE_SCHEMA_H
#define TABLE_SCHEMA_H

#include <string>
#include <vector>

#include "ColumnSchema.h"

namespace hegadb::catalog {

class TableSchema {
public:
    TableSchema(std::string name, std::vector<ColumnSchema> columns);
    std::string const& name() const;
    std::vector<ColumnSchema> const& columns() const;
    ColumnSchema const* findColumn(std::string const& name) const;

private:
    std::string name_;
    std::vector<ColumnSchema> columns_;
};

} // namespace hegadb

#endif // !TABLE_SCHEMA_H
