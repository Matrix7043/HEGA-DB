#include "hegadb/transport/CatalogClientTransport.h"
#include "hegadb/transport/grpc/GrpcCatalogClient.h"
#include <hegadb/transport/CatalogClientTransportFactory.h>
#include <memory>

namespace hegadb::transport {

std::unique_ptr<CatalogClientTransport> CatalogClientTransportFactory::createGrpc(std::string const& addr)
{
    return std::make_unique<grpc::GrpcCatalogClient>(addr);
}

}
