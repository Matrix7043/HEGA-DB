#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/catalog/service/CatalogManager.h"
#include <absl/strings/internal/str_format/extension.h>
#include <hegadb/catalog/service/CatalogService.h>

namespace hegadb::catalog::service {

namespace {

std::shared_ptr<CatalogSnapshot const> createInitialCatalog()
{
    using hegadb::types::DataType;

    TableSchema users(
        "users",
        { ColumnSchema {
              .name = "id",
              .type = DataType::Integer,
              .nullable = false,
              .ordinal = 0 },
            ColumnSchema {
                .name = "name",
                .type = DataType::String,
                .nullable = false,
                .ordinal = 1 },
            ColumnSchema {
                .name = "active",
                .type = DataType::Boolean,
                .nullable = false,
                .ordinal = 2 } });

    TableSchema orders(
        "orders",
        { ColumnSchema {
              .name = "id",
              .type = DataType::Integer,
              .nullable = false,
              .ordinal = 0 },
            ColumnSchema {
                .name = "user_id",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 1 },
            ColumnSchema {
                .name = "amount",
                .type = DataType::Float,
                .nullable = false,
                .ordinal = 2 } });

    TableSchema employees(
        "EMPLOYEES",
        { ColumnSchema {
              .name = "id",
              .type = DataType::Integer,
              .nullable = false,
              .ordinal = 0 },
            ColumnSchema {
                .name = "age",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 1 },
            ColumnSchema {
                .name = "salary",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 2 },
            ColumnSchema {
                .name = "dept",
                .type = DataType::String,
                .nullable = false,
                .ordinal = 3 } });

    TableSchema projects(
        "PROJECTS",
        { ColumnSchema {
              .name = "emp_id",
              .type = DataType::Integer,
              .nullable = false,
              .ordinal = 0 },
            ColumnSchema {
                .name = "project_id",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 1 },
            ColumnSchema {
                .name = "rating",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 2 } });

    std::unordered_map<std::string, TableSchema> tables;

    tables.emplace(users.name(), std::move(users));
    tables.emplace(orders.name(), std::move(orders));
    tables.emplace(employees.name(), std::move(employees));
    tables.emplace(projects.name(), std::move(projects));

    return std::make_shared<CatalogSnapshot>(
        1,
        std::move(tables));
}

}

CatalogService::CatalogService()
    : catalog_(createInitialCatalog())
{
}

CatalogSnapshot const& CatalogService::getCatalog() const
{
    return catalog_.snapshot();
}

SchemaVersion CatalogService::getCatalogVersion() const
{
    return catalog_.version();
}

TableSchema const* CatalogService::getTable(std::string const& table_name) const
{
    return catalog_.findTable(table_name);
}

CatalogManager const& CatalogService::catalog() const { return catalog_; }

SchemaVersion CatalogService::createTable(TableSchema table) { return catalog_.createTable(table); }

SchemaVersion CatalogService::dropTable(std::string const& table_name) { return catalog_.dropTable(table_name); }

}
