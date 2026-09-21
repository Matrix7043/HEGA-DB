#include "hegadb/transport/ServerTransportFactory.h"
#include "hegadb/transport/grpc/GrpcServer.h"
#include <memory>

namespace hegadb::transport {

std::unique_ptr<ServerTransport>
ServerTransportFactory::createGrpc(std::string const& addr)
{
    return std::make_unique<grpc::GrpcServer>(addr);
}

}
