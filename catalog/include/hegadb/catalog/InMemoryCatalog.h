#ifndef IN_MEMORY_CATALOG_H
#define IN_MEMORY_CATALOG_H

#include <memory>

#include "Catalog.h"

namespace hegadb::catalog {

class InMemoryCatalog final : public Catalog {
public:
    explicit InMemoryCatalog(std::shared_ptr<CatalogSnapshot const> snapshot);

    SchemaVersion version() const override;

    TableSchema const* findTable(std::string const& name) const override;

private:
    std::shared_ptr<CatalogSnapshot const> snapshot_;
};

} // namespace hegadb

#endif
