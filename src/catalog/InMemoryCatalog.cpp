#include "catalog/InMemoryCatalog.h"

#include <stdexcept>
#include <utility>

namespace hegadb {

InMemoryCatalog::InMemoryCatalog(
    std::shared_ptr<const CatalogSnapshot> snapshot)
    : snapshot_(std::move(snapshot)) {

  if (!snapshot_) {
    throw std::invalid_argument(
        "InMemoryCatalog requires a valid catalog snapshot");
  }
}

SchemaVersion InMemoryCatalog::version() const { return snapshot_->version(); }

const TableSchema *InMemoryCatalog::findTable(const std::string &name) const {
  return snapshot_->findTable(name);
}

} // namespace hegadb
