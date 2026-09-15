#include "hegadb/catalog/CatalogSnapshot.h"

#include <utility>

namespace hegadb::catalog {

CatalogSnapshot::CatalogSnapshot(
    SchemaVersion version, std::unordered_map<std::string, TableSchema> tables)
    : version_(version)
    , tables_(std::move(tables))
{
}

SchemaVersion CatalogSnapshot::version() const { return version_; }

TableSchema const* CatalogSnapshot::findTable(std::string const& name) const
{
    auto it = tables_.find(name);

    if (it == tables_.end())
        return nullptr;

    return &it->second;
}

} // namespace hegadb
