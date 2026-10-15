#ifndef CATALOG_SERVICE_H
#define CATALOG_SERVICE_H

#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/catalog/service/CatalogManager.h"
namespace hegadb::catalog::service {

class CatalogService {

public:
    CatalogService();

    SchemaVersion getCatalogVersion() const;

    TableSchema const* getTable(std::string const& table_name) const;

    CatalogSnapshot const& getCatalog() const;

    CatalogManager const& catalog() const;

    SchemaVersion createTable(TableSchema table);

    SchemaVersion dropTable(std::string const& table_name);

private:
    CatalogManager catalog_;
};

}

#endif // !CATALOG_SERVICE_H
