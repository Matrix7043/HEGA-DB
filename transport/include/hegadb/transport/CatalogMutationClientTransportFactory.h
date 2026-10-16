#ifndef CATALOG_MUTATION_CLIENT_TRANSPORT_FACTORY
#define CATALOG_MUTATION_CLIENT_TRANSPORT_FACTORY

#include "hegadb/transport/CatalogMutationClientTransport.h"
#include <memory>
namespace hegadb::transport {

class CatalogMutationClientTransportFactory {
public:
    static std::unique_ptr<CatalogMutationClientTransport>
    createGrpc(std::string const& addr);
};

}

#endif
