#ifndef COLUMN_SCHEMA_H
#define COLUMN_SCHEMA_H

#include <cstddef>
#include <string>

#include "hegadb/types/DataType.h"

namespace hegadb::catalog {

struct ColumnSchema {
    std::string name;
    types::DataType type;
    bool nullable;
    std::size_t ordinal;
};

} // namespace hegadb
#endif // !COLUMN_SCHEMA_H
