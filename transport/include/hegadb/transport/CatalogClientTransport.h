#ifndef CATALOG_CLIENT_TRANSPORT_H
#define CATALOG_CLIENT_TRANSPORT_H

#include <optional>

#include "types/CatalogTypes.h"

namespace hegadb::transport {

class CatalogClientTransport {
public:
    virtual ~CatalogClientTransport() = default;

    virtual CatalogVersion getCatalogVersion() = 0;

    virtual std::optional<CatalogTable>
    getTable(std::string const& table_name) = 0;

    virtual CatalogDescription getCatalog() = 0;
};

}

#endif // !CATALOG_CLIENT_TRANSPORT_H
