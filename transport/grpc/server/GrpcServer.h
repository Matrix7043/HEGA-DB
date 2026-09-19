#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include "HEGADBServiceImpl.h"
#include <memory>

namespace hegadb::transport::grpc {

class GrpcServer {
public:
    explicit GrpcServer(std::string address);

    void Start();
    void Wait();
    void Shutdown();

    [[nodiscard]]
    std::string const& address() const noexcept;

private:
    std::string address_;

    HEGADBServiceImpl service_;
    std::unique_ptr<::grpc::Server> server_;
};

}

#endif // !GRPC_SERVER_H
