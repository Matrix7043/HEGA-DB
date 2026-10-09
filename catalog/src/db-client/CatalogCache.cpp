#include "hegadb/catalog/Catalog.h"
#include "hegadb/catalog/CatalogAdapter.h"
#include "hegadb/catalog/db-client/InMemoryCatalog.h"
#include "hegadb/transport/CatalogClientTransport.h"
#include <hegadb/catalog/db-client/CatalogCache.h>
#include <memory>
#include <stdexcept>

namespace hegadb::catalog {

CatalogCache::CatalogCache(
    transport::CatalogClientTransport& client)
    : client_(client)
{
}

Catalog const& CatalogCache::catalog() const { return *catalog_; }

bool CatalogCache::refreshIfNeeded()
{

    auto remote_version = client_.getCatalogVersion();

    if (remote_version == catalog_->version()) {
        return false;
    }

    auto description = client_.getCatalog();

    auto snapshot = CatalogAdapter::fromTransport(description);

    catalog_ = std::make_shared<InMemoryCatalog>(std::move(snapshot));

    return true;
}

void CatalogCache::initialize()
{
    if (catalog_) {
        throw std::logic_error("CatalogCache is already initialized");
    }

    auto description = client_.getCatalog();
    auto snapshot = CatalogAdapter::fromTransport(description);

    catalog_ = std::make_shared<InMemoryCatalog>(std::move(snapshot));
}

}
