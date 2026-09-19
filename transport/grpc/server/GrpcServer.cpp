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

std::string const& GrpcServer::address() const noexcept { return address_; }

void GrpcServer::Start()
{
    ::grpc::ServerBuilder builder;

    int selected_port = 0;

    builder.AddListeningPort(
        address_,
        ::grpc::InsecureServerCredentials(),
        &selected_port);

    builder.RegisterService(&service_);

    server_ = builder.BuildAndStart();

    if (!server_) {
        throw std::runtime_error(
            "Failed to start gRPC server on " + address_);
    }

    if (selected_port == 0) {
        throw std::runtime_error(
            "Failed to obtain selected gRPC port");
    }

    auto const colon = address_.find(':');

    if (colon == std::string::npos) {
        throw std::runtime_error(
            "Invalid gRPC server address: " + address_);
    }

    address_ = address_.substr(0, colon + 1) + std::to_string(selected_port);

    std::cout
        << "HEGADB gRPC server listening on "
        << address_
        << '\n';
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
