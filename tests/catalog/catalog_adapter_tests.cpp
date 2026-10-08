#include "hegadb/catalog/CatalogAdapter.h"
#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/ColumnSchema.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/catalog/db-client/InMemoryCatalog.h"

#include "hegadb/transport/types/CatalogTypes.h"

#include "hegadb/types/DataType.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace {

using hegadb::catalog::CatalogAdapter;
using hegadb::catalog::CatalogSnapshot;
using hegadb::catalog::InMemoryCatalog;
using hegadb::types::DataType;

using hegadb::transport::CatalogColumn;
using hegadb::transport::CatalogDescription;
using hegadb::transport::CatalogTable;

CatalogDescription createCatalogDescription()
{
    CatalogDescription description;

    description.version = 7;

    description.tables = {
        CatalogTable {
            .name = "users",
            .columns = {
                CatalogColumn {
                    .name = "id",
                    .data_type = "INTEGER",
                    .nullable = false,
                    .ordinal = 0 },
                CatalogColumn {
                    .name = "name",
                    .data_type = "STRING",
                    .nullable = false,
                    .ordinal = 1 },
                CatalogColumn {
                    .name = "balance",
                    .data_type = "FLOAT",
                    .nullable = true,
                    .ordinal = 2 },
                CatalogColumn {
                    .name = "active",
                    .data_type = "BOOLEAN",
                    .nullable = false,
                    .ordinal = 3 } } }
    };

    return description;
}

void testVersion()
{
    auto description = createCatalogDescription();

    auto snapshot = CatalogAdapter::fromTransport(description);

    assert(snapshot != nullptr);
    assert(snapshot->version() == 7);

    std::cout << "[PASS] CatalogAdapter version\n";
}

void testTable()
{
    auto description = createCatalogDescription();

    auto snapshot = CatalogAdapter::fromTransport(description);

    auto table = snapshot->findTable("users");

    assert(table != nullptr);
    assert(table->name() == "users");

    std::cout << "[PASS] CatalogAdapter table\n";
}

void testColumns()
{
    auto description = createCatalogDescription();

    auto snapshot = CatalogAdapter::fromTransport(description);

    auto table = snapshot->findTable("users");

    assert(table != nullptr);
    assert(table->columns().size() == 4);

    auto const& id = table->columns()[0];

    assert(id.name == "id");
    assert(id.type == DataType::Integer);
    assert(id.nullable == false);
    assert(id.ordinal == 0);

    auto const& name = table->columns()[1];

    assert(name.name == "name");
    assert(name.type == DataType::String);
    assert(name.nullable == false);
    assert(name.ordinal == 1);

    auto const& balance = table->columns()[2];

    assert(balance.name == "balance");
    assert(balance.type == DataType::Float);
    assert(balance.nullable == true);
    assert(balance.ordinal == 2);

    auto const& active = table->columns()[3];

    assert(active.name == "active");
    assert(active.type == DataType::Boolean);
    assert(active.nullable == false);
    assert(active.ordinal == 3);

    std::cout << "[PASS] CatalogAdapter columns\n";
}

void testInMemoryCatalog()
{
    auto description = createCatalogDescription();

    auto snapshot = CatalogAdapter::fromTransport(description);

    InMemoryCatalog catalog(snapshot);

    assert(catalog.version() == 7);

    auto table = catalog.findTable("users");

    assert(table != nullptr);
    assert(table->name() == "users");

    auto column = table->findColumn("balance");

    assert(column != nullptr);
    assert(column->type == DataType::Float);
    assert(column->nullable == true);

    std::cout << "[PASS] InMemoryCatalog integration\n";
}

void testMissingTable()
{
    auto description = createCatalogDescription();

    auto snapshot = CatalogAdapter::fromTransport(description);

    assert(snapshot->findTable("does_not_exist") == nullptr);

    std::cout << "[PASS] Missing table\n";
}

void testDuplicateTable()
{
    auto description = createCatalogDescription();

    description.tables.push_back(
        CatalogTable {
            .name = "users",
            .columns = { } });

    bool threw = false;

    try {
        CatalogAdapter::fromTransport(description);
    } catch (std::runtime_error const&) {
        threw = true;
    }

    assert(threw);

    std::cout << "[PASS] Duplicate table rejected\n";
}

void testUnsupportedDataType()
{
    CatalogDescription description;

    description.version = 1;

    description.tables = {
        CatalogTable {
            .name = "invalid",
            .columns = {
                CatalogColumn {
                    .name = "value",
                    .data_type = "INVALID_TYPE",
                    .nullable = false,
                    .ordinal = 0 } } }
    };

    bool threw = false;

    try {
        CatalogAdapter::fromTransport(description);
    } catch (std::runtime_error const&) {
        threw = true;
    }

    assert(threw);

    std::cout << "[PASS] Unsupported data type rejected\n";
}

} // namespace

int main()
{
    testVersion();
    testTable();
    testColumns();
    testInMemoryCatalog();
    testMissingTable();
    testDuplicateTable();
    testUnsupportedDataType();

    std::cout
        << "\nAll catalog adapter tests passed.\n";

    return 0;
}
