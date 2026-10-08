#ifndef HEGADB_CATALOG_ADAPTER_H
#define HEGADB_CATALOG_ADAPTER_H

#include "hegadb/catalog/CatalogSnapshot.h"

#include <hegadb/transport/types/CatalogTypes.h>
#include <memory>

namespace hegadb::catalog {

class CatalogAdapter {

public:
    static std::shared_ptr<CatalogSnapshot const>
    fromTransport(transport::CatalogDescription const& description);
};

}

#endif
