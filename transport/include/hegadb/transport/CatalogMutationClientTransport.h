#ifndef CATALOG_MUTATION_CLIENT_TRANSPORT
#define CATALOG_MUTATION_CLIENT_TRANSPORT

#include "hegadb/transport/types/CatalogTypes.h"

namespace hegadb::transport {

class CatalogMutationClientTransport {
public:
    virtual ~CatalogMutationClientTransport() = default;

    virtual CatalogVersion getCatalogVersion() = 0;
    virtual void createTable(CatalogTable table) = 0;
    virtual void dropTable(std::string const& table_name) = 0;
};

}

#endif // !CATALOG_MUTATION_CLIENT_TRANSPORT
