#ifndef CATALOG_CACHE_H
#define CATALOG_CACHE_H

#include "hegadb/catalog/Catalog.h"
#include "hegadb/catalog/db-client/InMemoryCatalog.h"
#include "hegadb/transport/CatalogClientTransport.h"
#include <memory>

namespace hegadb::catalog {

class CatalogCache {
public:
    explicit CatalogCache(
        transport::CatalogClientTransport& client);

    Catalog const& catalog() const;

    bool refreshIfNeeded();

    void initialize();

private:
    transport::CatalogClientTransport& client_;
    std::shared_ptr<InMemoryCatalog> catalog_;
};

}

#endif // !CATALOG_CACHE_H
