#ifndef HEGADB_TRANSPORT_CATALOG_TYPES_H
#define HEGADB_TRANSPORT_CATALOG_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

namespace hegadb::transport {

using CatalogVersion = std::uint64_t;

struct CatalogColumn {
    std::string name;
    std::string data_type;
    bool nullable;
    std::uint64_t ordinal;
};

struct CatalogTable {
    std::string name;
    std::vector<CatalogColumn> columns;
};

struct CatalogDescription {
    CatalogVersion version;
    std::vector<CatalogTable> tables;
};

} // namespace hegadb::transport

#endif
