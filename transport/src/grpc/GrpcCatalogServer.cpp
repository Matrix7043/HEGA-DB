#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <hegadb/transport/grpc/CatalogServiceImpl.h>
#include <hegadb/transport/grpc/GrpcCatalogServer.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace hegadb::transport::grpc {

GrpcCatalogServer::GrpcCatalogServer(
    std::string address,
    std::shared_ptr<CatalogServiceImpl> service)
    : address_(std::move(address))
    , service_(std::move(service))
{
    if (address_.empty()) {
        throw std::invalid_argument("GrpcCatalogServer address cannot be empty");
    }
    if (!service_) {
        throw std::invalid_argument("GrpcCatalogServer service cannot be null");
    }
}

GrpcCatalogServer::~GrpcCatalogServer()
{
    shutdown();
}

void GrpcCatalogServer::start()
{
    if (server_) {
        return;
    }

    ::grpc::ServerBuilder builder;

    builder.AddListeningPort(address_, ::grpc::InsecureServerCredentials());

    builder.RegisterService(service_.get());

    server_ = builder.BuildAndStart();

    if (!server_) {
        throw std::runtime_error("Failed to start gRPC CatalogServer at " + address_);
    }
}

void GrpcCatalogServer::shutdown()
{
    if (!server_) {
        return;
    }

    server_->Shutdown();
    server_.reset();
}

}
