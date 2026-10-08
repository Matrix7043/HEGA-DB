#include "hegadb/catalog/CatalogAdapter.h"

#include "hegadb/types/DataType.h"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace hegadb::catalog {

namespace {

types::DataType parseDataType(std::string const& type)
{
    if (type == "INTEGER")
        return types::DataType::Integer;

    if (type == "FLOAT")
        return types::DataType::Float;

    if (type == "STRING")
        return types::DataType::String;

    if (type == "BOOLEAN")
        return types::DataType::Boolean;

    throw std::runtime_error(
        "Unsupported catalog data type: " + type);
}

ColumnSchema convertColumn(
    transport::CatalogColumn const& column)
{
    return ColumnSchema {
        .name = column.name,
        .type = parseDataType(column.data_type),
        .nullable = column.nullable,
        .ordinal = column.ordinal
    };
}

TableSchema convertTable(
    transport::CatalogTable const& table)
{
    std::vector<ColumnSchema> columns;
    columns.reserve(table.columns.size());

    for (auto const& column : table.columns) {
        columns.push_back(convertColumn(column));
    }

    return TableSchema(
        table.name,
        std::move(columns));
}

} // namespace

std::shared_ptr<CatalogSnapshot const>
CatalogAdapter::fromTransport(
    transport::CatalogDescription const& description)
{
    std::unordered_map<std::string, TableSchema> tables;
    tables.reserve(description.tables.size());

    for (auto const& table : description.tables) {
        auto [it, inserted] = tables.emplace(
            table.name,
            convertTable(table));

        if (!inserted) {
            throw std::runtime_error(
                "Duplicate table in catalog description: " + table.name);
        }
    }

    return std::make_shared<CatalogSnapshot>(
        description.version,
        std::move(tables));
}

} // namespace hegadb::catalog
