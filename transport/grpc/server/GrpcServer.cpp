#include "GrpcServer.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace hegadb::transport::grpc {

GrpcServer::GrpcServer(std::string address)
    : address_(std::move(address))
{
}

void GrpcServer::Start()
{
    ::grpc::ServerBuilder builder;

    builder.RegisterService(&service_);

    server_ = builder.BuildAndStart();

    if (!server_) {
        throw std::runtime_error("Failed to start gRPC server on " + address_);
    }

    std::cout << "HEGADB gRPC server listening on " << address_ << "\n";
}

void GrpcServer::Wait()
{
    if (server_) {
        server_->Wait();
    }
}

void GrpcServer::Shutdown()
{
    if (server_) {
        server_->Shutdown();
    }
}

}
