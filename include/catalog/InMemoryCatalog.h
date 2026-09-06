#ifndef IN_MEMORY_CATALOG_H
#define IN_MEMORY_CATALOG_H

#include <memory>

#include "Catalog.h"

namespace hegadb {

class InMemoryCatalog final : public Catalog {
public:
  explicit InMemoryCatalog(std::shared_ptr<const CatalogSnapshot> snapshot);

  SchemaVersion version() const override;

  const TableSchema *findTable(const std::string &name) const override;

private:
  std::shared_ptr<const CatalogSnapshot> snapshot_;
};

} // namespace hegadb

#endif
