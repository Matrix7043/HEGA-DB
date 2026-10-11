#ifndef CATALOG_SERVER_TRANSPORT_FACTORY_H
#define CATALOG_SERVER_TRANSPORT_FACTORY_H

#include "hegadb/transport/CatalogServerTransport.h"
#include "hegadb/transport/grpc/CatalogServiceImpl.h"
#include <memory>

namespace hegadb::transport {

class CatalogServerTransportFactory {

public:
    static std::unique_ptr<CatalogServerTransport> createGrpc(
        std::string const& addr,
        std::shared_ptr<grpc::CatalogServiceImpl> service);
};

}

#endif // !CATALOG_SERVER_TRANSPORT_FACTORY_H
