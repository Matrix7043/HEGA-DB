#ifndef CATALOG_MANAGER_H
#define CATALOG_MANAGER_H

#include <memory>
#include <string>

#include "hegadb/catalog/Catalog.h"
#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/TableSchema.h"

namespace hegadb::catalog {

class CatalogManager final : public Catalog {

public:
    explicit CatalogManager(std::shared_ptr<CatalogSnapshot const> initial_snapshot);

    SchemaVersion version() const override;

    TableSchema const* findTable(std::string const& name) const override;

    void createTable(TableSchema table);

    void dropTable(std::string const& name);

private:
    std::shared_ptr<CatalogSnapshot const> snapshot_;
};

}
#endif // !CATALOG_MANAGER_H
