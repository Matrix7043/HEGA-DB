#ifndef TABLE_SCHEMA_H
#define TABLE_SCHEMA_H

#include <string>
#include <vector>

#include "ColumnSchema.h"

namespace hegadb {

class TableSchema {
public:
  TableSchema(std::string name, std::vector<ColumnSchema> columns);
  const std::string &name() const;
  const std::vector<ColumnSchema> &columns() const;
  const ColumnSchema *findColumn(const std::string &name) const;

private:
  std::string name_;
  std::vector<ColumnSchema> columns_;
};
} // namespace hegadb

#endif // !TABLE_SCHEMA_H
