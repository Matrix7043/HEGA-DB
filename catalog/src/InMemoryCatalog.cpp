#include "hegadb/catalog/InMemoryCatalog.h"

#include <stdexcept>
#include <utility>

namespace hegadb::catalog {

InMemoryCatalog::InMemoryCatalog(
    std::shared_ptr<CatalogSnapshot const> snapshot)
    : snapshot_(std::move(snapshot))
{

    if (!snapshot_) {
        throw std::invalid_argument(
            "InMemoryCatalog requires a valid catalog snapshot");
    }
}

SchemaVersion InMemoryCatalog::version() const { return snapshot_->version(); }

TableSchema const* InMemoryCatalog::findTable(std::string const& name) const
{
    return snapshot_->findTable(name);
}

} // namespace hegadb
