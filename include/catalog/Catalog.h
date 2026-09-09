#ifndef CATALOG_H
#define CATALOG_H

#include <string>

#include "CatalogSnapshot.h"

namespace hegadb {

class Catalog {
public:
  virtual ~Catalog() = default;
  virtual SchemaVersion version() const = 0;
  virtual const TableSchema *findTable(const std::string &name) const = 0;
};

} // namespace hegadb

#endif // !CATALOG_H
