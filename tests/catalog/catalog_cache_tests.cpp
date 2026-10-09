#include "hegadb/catalog/db-client/CatalogCache.h"

#include "hegadb/transport/CatalogClientTransport.h"
#include "hegadb/transport/types/CatalogTypes.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <string>

namespace {

using hegadb::catalog::CatalogCache;
using hegadb::transport::CatalogClientTransport;
using hegadb::transport::CatalogDescription;
using hegadb::transport::CatalogTable;

class FakeCatalogClient final
    : public CatalogClientTransport {
public:
    CatalogDescription remote_catalog;

    int version_requests = 0;
    int catalog_requests = 0;

    hegadb::transport::CatalogVersion getCatalogVersion() override
    {
        ++version_requests;
        return remote_catalog.version;
    }

    std::optional<
        hegadb::transport::CatalogTable>
    getTable(std::string const& table_name) override
    {
        for (auto const& table : remote_catalog.tables) {
            if (table.name == table_name) {
                return table;
            }
        }

        return std::nullopt;
    }

    CatalogDescription getCatalog() override
    {
        ++catalog_requests;
        return remote_catalog;
    }
};

CatalogDescription makeCatalog(
    std::uint64_t version,
    std::string table_name)
{
    CatalogDescription description;

    description.version = version;

    description.tables = {
        CatalogTable {
            .name = std::move(table_name),
            .columns = { } }
    };

    return description;
}

void testInitialLoad()
{
    FakeCatalogClient client;

    client.remote_catalog = makeCatalog(1, "users");

    CatalogCache cache(client);

    cache.initialize();

    assert(cache.catalog().version() == 1);
    assert(cache.catalog().findTable("users") != nullptr);

    assert(client.catalog_requests == 1);

    std::cout << "[PASS] Initial catalog load\n";
}

void testNoRefreshWhenVersionMatches()
{
    FakeCatalogClient client;

    client.remote_catalog = makeCatalog(1, "users");

    CatalogCache cache(client);

    cache.initialize();

    int catalog_requests_before = client.catalog_requests;

    bool refreshed = cache.refreshIfNeeded();

    assert(!refreshed);

    assert(cache.catalog().version() == 1);

    assert(client.catalog_requests == catalog_requests_before);

    std::cout << "[PASS] No refresh when version matches\n";
}

void testRefreshWhenVersionChanges()
{
    FakeCatalogClient client;

    client.remote_catalog = makeCatalog(1, "users");

    CatalogCache cache(client);

    cache.initialize();

    client.remote_catalog = makeCatalog(2, "customers");

    bool refreshed = cache.refreshIfNeeded();

    assert(refreshed);

    assert(cache.catalog().version() == 2);

    assert(
        cache.catalog().findTable("customers") != nullptr);

    assert(
        cache.catalog().findTable("users") == nullptr);

    std::cout << "[PASS] Refresh when version changes\n";
}

void testSecondRefreshNotNeeded()
{
    FakeCatalogClient client;

    client.remote_catalog = makeCatalog(1, "users");

    CatalogCache cache(client);

    cache.initialize();

    client.remote_catalog = makeCatalog(2, "customers");

    bool first_refresh = cache.refreshIfNeeded();

    assert(first_refresh);

    int catalog_requests = client.catalog_requests;

    bool second_refresh = cache.refreshIfNeeded();

    assert(!second_refresh);

    assert(client.catalog_requests == catalog_requests);

    std::cout << "[PASS] Second refresh not needed\n";
}

} // namespace

int main()
{
    testInitialLoad();
    testNoRefreshWhenVersionMatches();
    testRefreshWhenVersionChanges();
    testSecondRefreshNotNeeded();

    std::cout
        << "\nAll catalog cache tests passed.\n";

    return 0;
}
