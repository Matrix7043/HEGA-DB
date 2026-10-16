#include "hegadb/transport/CatalogMutationClientTransportFactory.h"
#include "hegadb/transport/CatalogMutationClientTransport.h"
#include "hegadb/transport/grpc/GrpcCatalogMutationClient.h"
#include <memory>

namespace hegadb::transport {

std::unique_ptr<CatalogMutationClientTransport>
CatalogMutationClientTransportFactory::createGrpc(std::string const& addr)
{
    return std::make_unique<grpc::GrpcCatalogMutationClient>(addr);
}

}
