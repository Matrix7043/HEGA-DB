#ifndef CATALOG_SNAPSHOT_H
#define CATALOG_SNAPSHOT_H

#include "TableSchema.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace hegadb::catalog {

using SchemaVersion = std::uint64_t;

class CatalogSnapshot {

public:
    CatalogSnapshot(SchemaVersion version,
        std::unordered_map<std::string, TableSchema> tables);

    SchemaVersion version() const;

    TableSchema const* findTable(std::string const& name) const;

private:
    SchemaVersion version_;
    std::unordered_map<std::string, TableSchema> tables_;
};

} // namespace hegadb

#endif // CATALOG_SNAPSHOT_H
