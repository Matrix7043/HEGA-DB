#ifndef HEGADB_CATALOG_ADAPTER_H
#define HEGADB_CATALOG_ADAPTER_H

#include "hegadb/catalog/CatalogSnapshot.h"
#include "hegadb/catalog/TableSchema.h"

#include <hegadb/transport/types/CatalogTypes.h>
#include <memory>

namespace hegadb::catalog {

class CatalogAdapter {

public:
    static std::shared_ptr<CatalogSnapshot const>
    fromTransport(transport::CatalogDescription const& description);

    static transport::CatalogDescription
    toTransport(CatalogSnapshot const& snapshot);

    static transport::CatalogTable
    toTransport(TableSchema const& table);

    static TableSchema fromTransport(transport::CatalogTable const& table);
};

}

#endif
