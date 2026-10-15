#include "hegadb/transport/CatalogServerTransport.h"
#include "hegadb/transport/grpc/CatalogServiceImpl.h"
#include "hegadb/transport/grpc/GrpcCatalogServer.h"
#include <hegadb/transport/CatalogServerTransportFactory.h>
#include <memory>

namespace hegadb::transport {

std::unique_ptr<CatalogServerTransport> CatalogServerTransportFactory::createGrpc(
    std::string const& addr, std::shared_ptr<grpc::CatalogServiceImpl> service)
{
    return std::make_unique<grpc::GrpcCatalogServer>(addr, std::move(service));
}

}
