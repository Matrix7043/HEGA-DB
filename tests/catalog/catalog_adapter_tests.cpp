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
using hegadb::catalog::ColumnSchema;
using hegadb::catalog::TableSchema;

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

    hegadb::catalog::InMemoryCatalog catalog(snapshot);

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

void testTransportToCatalog()
{
    CatalogDescription description;

    description.version = 42;

    description.tables.push_back(
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
                    .nullable = true,
                    .ordinal = 1 },
                CatalogColumn {
                    .name = "score",
                    .data_type = "FLOAT",
                    .nullable = true,
                    .ordinal = 2 },
                CatalogColumn {
                    .name = "active",
                    .data_type = "BOOLEAN",
                    .nullable = false,
                    .ordinal = 3 } } });

    auto snapshot = CatalogAdapter::fromTransport(description);

    assert(snapshot != nullptr);
    assert(snapshot->version() == 42);

    auto const* users = snapshot->findTable("users");

    assert(users != nullptr);
    assert(users->name() == "users");

    auto const& columns = users->columns();

    assert(columns.size() == 4);

    assert(columns[0].name == "id");
    assert(columns[0].type == DataType::Integer);
    assert(!columns[0].nullable);
    assert(columns[0].ordinal == 0);

    assert(columns[1].name == "name");
    assert(columns[1].type == DataType::String);
    assert(columns[1].nullable);
    assert(columns[1].ordinal == 1);

    assert(columns[2].name == "score");
    assert(columns[2].type == DataType::Float);
    assert(columns[2].nullable);
    assert(columns[2].ordinal == 2);

    assert(columns[3].name == "active");
    assert(columns[3].type == DataType::Boolean);
    assert(!columns[3].nullable);
    assert(columns[3].ordinal == 3);

    std::cout << "[PASS] transport -> catalog\n";
}

void testCatalogToTransport()
{
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
                .nullable = true,
                .ordinal = 1 },
            ColumnSchema {
                .name = "score",
                .type = DataType::Float,
                .nullable = true,
                .ordinal = 2 },
            ColumnSchema {
                .name = "active",
                .type = DataType::Boolean,
                .nullable = false,
                .ordinal = 3 } });

    auto result = CatalogAdapter::toTransport(users);

    assert(result.name == "users");
    assert(result.columns.size() == 4);

    assert(result.columns[0].name == "id");
    assert(result.columns[0].data_type == "INTEGER");
    assert(!result.columns[0].nullable);
    assert(result.columns[0].ordinal == 0);

    assert(result.columns[1].name == "name");
    assert(result.columns[1].data_type == "STRING");
    assert(result.columns[1].nullable);
    assert(result.columns[1].ordinal == 1);

    assert(result.columns[2].name == "score");
    assert(result.columns[2].data_type == "FLOAT");
    assert(result.columns[2].nullable);
    assert(result.columns[2].ordinal == 2);

    assert(result.columns[3].name == "active");
    assert(result.columns[3].data_type == "BOOLEAN");
    assert(!result.columns[3].nullable);
    assert(result.columns[3].ordinal == 3);

    std::cout << "[PASS] catalog -> transport\n";
}

void testSnapshotToTransport()
{
    std::unordered_map<std::string, TableSchema> tables;

    tables.emplace(
        "users",
        TableSchema(
            "users",
            { ColumnSchema {
                .name = "id",
                .type = DataType::Integer,
                .nullable = false,
                .ordinal = 0 } }));

    tables.emplace(
        "orders",
        TableSchema(
            "orders",
            { ColumnSchema {
                  .name = "id",
                  .type = DataType::Integer,
                  .nullable = false,
                  .ordinal = 0 },
                ColumnSchema {
                    .name = "amount",
                    .type = DataType::Float,
                    .nullable = false,
                    .ordinal = 1 } }));

    CatalogSnapshot snapshot(
        99,
        std::move(tables));

    auto result = CatalogAdapter::toTransport(snapshot);

    assert(result.version == 99);
    assert(result.tables.size() == 2);

    bool found_users = false;
    bool found_orders = false;

    for (auto const& table : result.tables) {
        if (table.name == "users") {
            found_users = true;

            assert(table.columns.size() == 1);
            assert(table.columns[0].data_type == "INTEGER");
        }

        if (table.name == "orders") {
            found_orders = true;

            assert(table.columns.size() == 2);
            assert(table.columns[0].data_type == "INTEGER");
            assert(table.columns[1].data_type == "FLOAT");
        }
    }

    assert(found_users);
    assert(found_orders);

    std::cout << "[PASS] snapshot -> transport\n";
}

void testRoundTrip()
{
    CatalogDescription original;

    original.version = 123;

    original.tables.push_back(
        CatalogTable {
            .name = "products",
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
                    .name = "price",
                    .data_type = "FLOAT",
                    .nullable = true,
                    .ordinal = 2 } } });

    auto snapshot = CatalogAdapter::fromTransport(original);

    auto result = CatalogAdapter::toTransport(*snapshot);

    assert(result.version == original.version);
    assert(result.tables.size() == original.tables.size());

    auto const& original_table = original.tables[0];
    auto const& result_table = result.tables[0];

    assert(result_table.name == original_table.name);
    assert(result_table.columns.size() == original_table.columns.size());

    for (std::size_t i = 0;
        i < original_table.columns.size();
        ++i) {

        auto const& expected = original_table.columns[i];
        auto const& actual = result_table.columns[i];

        assert(actual.name == expected.name);
        assert(actual.data_type == expected.data_type);
        assert(actual.nullable == expected.nullable);
        assert(actual.ordinal == expected.ordinal);
    }

    std::cout << "[PASS] catalog transport round-trip\n";
}

void testDuplicateTableRejected()
{
    CatalogDescription description;

    description.version = 1;

    description.tables.push_back(
        CatalogTable {
            .name = "users",
            .columns = { } });

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

    std::cout << "[PASS] duplicate table rejected\n";
}

void testUnsupportedDataTypeRejected()
{
    CatalogDescription description;

    description.version = 1;

    description.tables.push_back(
        CatalogTable {
            .name = "users",
            .columns = {
                CatalogColumn {
                    .name = "id",
                    .data_type = "DATE",
                    .nullable = false,
                    .ordinal = 0 } } });

    bool threw = false;

    try {
        CatalogAdapter::fromTransport(description);
    } catch (std::runtime_error const&) {
        threw = true;
    }

    assert(threw);

    std::cout << "[PASS] unsupported data type rejected\n";
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
    testTransportToCatalog();
    testCatalogToTransport();
    testSnapshotToTransport();
    testRoundTrip();
    testDuplicateTableRejected();
    testUnsupportedDataTypeRejected();

    std::cout
        << "\nAll catalog adapter tests passed.\n";

    return 0;
}
