#ifndef COLUMN_SCHEMA_H
#define COLUMN_SCHEMA_H

#include <cstddef>
#include <string>

#include "DataType.h"

namespace hegadb {

struct ColumnSchema {
  std::string name;
  DataType type;
  bool nullable;
  std::size_t ordinal;
};

} // namespace hegadb
#endif // !COLUMN_SCHEMA_H
