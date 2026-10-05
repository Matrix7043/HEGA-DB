#include "hegadb/catalog/CatalogManager.h"
#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/TableSchema.h"
#include <memory>
#include <stdexcept>
#include <utility>

namespace hegadb::catalog {

CatalogManager::CatalogManager(
    std::shared_ptr<CatalogSnapshot const> initial_snapshot)
    : snapshot_(std::move(initial_snapshot))
{
    if (!snapshot_) {
        throw std::invalid_argument("CatalogManager requires a valid catalog snapshot");
    }
}

SchemaVersion CatalogManager::version() const
{
    return snapshot_->version();
}

TableSchema const* CatalogManager::findTable(std::string const& name) const
{
    return snapshot_->findTable(name);
}

void CatalogManager::createTable(TableSchema table)
{
    auto const& table_name = table.name();

    if (snapshot_->findTable(table_name) != nullptr) {
        throw std::runtime_error("Table already exists: " + table_name);
    }

    auto tables = snapshot_->tables();

    tables.emplace(table_name, std::move(table));

    auto new_snapshot = std::make_shared<CatalogSnapshot>(
        snapshot_->version() + 1,
        std::move(tables));

    snapshot_ = std::move(new_snapshot);
}

void CatalogManager::dropTable(std::string const& name)
{
    auto const& exisiting_tables = snapshot_->findTable(name);
    if (exisiting_tables == nullptr) {
        throw std::runtime_error("Table does not exist: " + name);
    }

    auto tables = snapshot_->tables();

    tables.erase(name);

    auto new_snapshot = std::make_shared<CatalogSnapshot>(
        snapshot_->version() + 1,
        std::move(tables));

    snapshot_ = std::move(new_snapshot);
}

}
