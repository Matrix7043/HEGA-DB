#include <catalog/CatalogSnapshot.h>
#include <utility>

namespace hegadb {

CatalogSnapshot::CatalogSnapshot(
    SchemaVersion version, std::unordered_map<std::string, TableSchema> tables)
    : version_(version), tables_(std::move(tables)) {}

SchemaVersion CatalogSnapshot::version() const { return version_; }

const TableSchema *CatalogSnapshot::findTable(const std::string &name) const {
  auto it = tables_.find(name);

  if (it == tables_.end())
    return nullptr;

  return &it->second;
}

} // namespace hegadb
