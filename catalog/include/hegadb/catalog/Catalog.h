#ifndef CATALOG_H
#define CATALOG_H

#include <string>

#include "CatalogSnapshot.h"

namespace hegadb::catalog {

class Catalog {
public:
    virtual ~Catalog() = default;
    virtual SchemaVersion version() const = 0;
    virtual TableSchema const* findTable(std::string const& name) const = 0;
};

} // namespace hegadb

#endif // !CATALOG_H
