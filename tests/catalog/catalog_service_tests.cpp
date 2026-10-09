#include "hegadb/catalog/service/CatalogService.h"
#include "hegadb/types/DataType.h"

#include <cassert>
#include <iostream>
#include <string>

namespace {

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

    std::cout << "[PASS] missing table\n";
}

void testCompleteCatalog()
{
    CatalogService service;

    auto const& snapshot = service.getCatalog();

    assert(snapshot.version() == 1);
    assert(snapshot.tables().size() == 2);

    auto const* users = snapshot.findTable("users");
    auto const* orders = snapshot.findTable("orders");

    assert(users != nullptr);
    assert(orders != nullptr);

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

} // namespace

int main()
{
    testInitialCatalogVersion();
    testUsersTable();
    testOrdersTable();
    testMissingTable();
    testCompleteCatalog();
    testCatalogManagerAccess();

    std::cout << "\nAll CatalogService tests passed.\n";
    return 0;
}
