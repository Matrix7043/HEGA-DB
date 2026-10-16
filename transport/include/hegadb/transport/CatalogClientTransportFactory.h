#ifndef CATALOG_CLIENT_TRANSPORT_FACTORY_H
#define CATALOG_CLIENT_TRANSPORT_FACTORY_H

#include "hegadb/transport/CatalogClientTransport.h"
#include <memory>
namespace hegadb::transport {

class CatalogClientTransportFactory {
public:
    static std::unique_ptr<CatalogClientTransport> createGrpc(
        std::string const& addr);
};

}

#endif // !CATALOG_CLIENT_TRANSPORT_FACTORY_H
