#include "hegadb/catalog/TableSchema.h"
#include "hegadb/catalog/service/CatalogService.h"
#include "hegadb/types/DataType.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using hegadb::catalog::ColumnSchema;
using hegadb::catalog::TableSchema;
using hegadb::catalog::service::CatalogService;
using hegadb::types::DataType;

void testInitialCatalogVersion()
{
    CatalogService service;

    assert(service.getCatalogVersion() == 1);

    std::cout << "[PASS] initial catalog version\n";
}

void testUsersTable()
{
    CatalogService service;

    auto const* users = service.getTable("users");

    assert(users != nullptr);
    assert(users->name() == "users");

    auto const& columns = users->columns();

    assert(columns.size() == 3);

    assert(columns[0].name == "id");
    assert(columns[0].type == DataType::Integer);
    assert(!columns[0].nullable);
    assert(columns[0].ordinal == 0);

    assert(columns[1].name == "name");
    assert(columns[1].type == DataType::String);
    assert(!columns[1].nullable);
    assert(columns[1].ordinal == 1);

    assert(columns[2].name == "active");
    assert(columns[2].type == DataType::Boolean);
    assert(!columns[2].nullable);
    assert(columns[2].ordinal == 2);

    std::cout << "[PASS] users table\n";
}

void testOrdersTable()
{
    CatalogService service;

    auto const* orders = service.getTable("orders");

    assert(orders != nullptr);
    assert(orders->name() == "orders");

    auto const& columns = orders->columns();

    assert(columns.size() == 3);

    assert(columns[0].name == "id");
    assert(columns[0].type == DataType::Integer);
    assert(!columns[0].nullable);
    assert(columns[0].ordinal == 0);

    assert(columns[1].name == "user_id");
    assert(columns[1].type == DataType::Integer);
    assert(!columns[1].nullable);
    assert(columns[1].ordinal == 1);

    assert(columns[2].name == "amount");
    assert(columns[2].type == DataType::Float);
    assert(!columns[2].nullable);
    assert(columns[2].ordinal == 2);

    std::cout << "[PASS] orders table\n";
}

void testMissingTable()
{
    CatalogService service;

    auto const* table = service.getTable("does_not_exist");

    assert(table == nullptr);

    std::cout << "[PASS] missing table lookup\n";
}

void testCompleteCatalog()
{
    CatalogService service;

    auto const& snapshot = service.getCatalog();

    assert(snapshot.version() == 1);
    assert(snapshot.tables().size() == 2);

    assert(snapshot.findTable("users") != nullptr);
    assert(snapshot.findTable("orders") != nullptr);

    std::cout << "[PASS] complete catalog\n";
}

void testCatalogManagerAccess()
{
    CatalogService service;

    auto const& manager = service.catalog();

    assert(manager.version() == 1);
    assert(manager.findTable("users") != nullptr);
    assert(manager.findTable("orders") != nullptr);

    std::cout << "[PASS] catalog manager access\n";
}

TableSchema makeTestTable()
{
    std::vector<ColumnSchema> columns = {
        { "id", DataType::Integer, false, 0 },
        { "label", DataType::String, false, 1 }
    };

    return TableSchema("mutation_test", std::move(columns));
}

void testCreateTable()
{
    CatalogService service;

    auto version = service.createTable(makeTestTable());

    assert(version == 2);
    assert(service.getCatalogVersion() == 2);

    auto const* table = service.getTable("mutation_test");

    assert(table != nullptr);
    assert(table->name() == "mutation_test");
    assert(table->columns().size() == 2);
    assert(table->columns()[0].name == "id");
    assert(table->columns()[1].name == "label");

    std::cout << "[PASS] create table and increment version\n";
}

void testDuplicateTableCreation()
{
    CatalogService service;

    service.createTable(makeTestTable());

    bool threw = false;

    try {
        service.createTable(makeTestTable());
    } catch (std::logic_error const&) {
        threw = true;
    }

    assert(threw);
    assert(service.getCatalogVersion() == 2);
    assert(service.getTable("mutation_test") != nullptr);

    std::cout << "[PASS] duplicate table creation rejected\n";
}

void testDropTable()
{
    CatalogService service;

    service.createTable(makeTestTable());

    auto version = service.dropTable("mutation_test");

    assert(version == 3);
    assert(service.getCatalogVersion() == 3);
    assert(service.getTable("mutation_test") == nullptr);

    // Dropping the test table must not affect the initial tables.
    assert(service.getTable("users") != nullptr);
    assert(service.getTable("orders") != nullptr);

    std::cout << "[PASS] drop table and increment version\n";
}

void testDropMissingTable()
{
    CatalogService service;

    bool threw = false;

    try {
        service.dropTable("does_not_exist");
    } catch (std::logic_error const&) {
        threw = true;
    }

    assert(threw);
    assert(service.getCatalogVersion() == 1);
    assert(service.getTable("users") != nullptr);
    assert(service.getTable("orders") != nullptr);

    std::cout << "[PASS] deletion of missing table rejected\n";
}

} // namespace

int main()
{
    testInitialCatalogVersion();
    testUsersTable();
    testOrdersTable();
    testMissingTable();
    testCompleteCatalog();
    testCatalogManagerAccess();

    testCreateTable();
    testDuplicateTableCreation();
    testDropTable();
    testDropMissingTable();

    std::cout << "\nAll CatalogService tests passed.\n";
    return 0;
}
