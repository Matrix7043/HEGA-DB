#include "hegadb/transport/ClientTransportFactory.h"

#include "hegadb/transport/grpc/GrpcClient.h"

namespace hegadb::transport {

std::unique_ptr<ClientTransport>
ClientTransportFactory::createGrpc(
    std::string const& address)
{
    return std::make_unique<
        grpc::GrpcClient>(address);
}

} // namespace hegadb::transport
