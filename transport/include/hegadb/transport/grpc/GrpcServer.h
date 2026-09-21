#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include "HEGADBServiceImpl.h"
#include "hegadb/transport/ServerTransport.h"

#include <memory>

namespace hegadb::transport::grpc {

class GrpcServer final : public transport::ServerTransport {
public:
    explicit GrpcServer(std::string address);

    void setRequestHandler(RequestHandler handler) override;

    void start() override;
    void wait() override;
    void shutdown() override;

    [[nodiscard]]
    std::string const& address() const noexcept;

private:
    std::string address_;
    int selected_port_ = 0;

    HEGADBServiceImpl service_;
    std::unique_ptr<::grpc::Server> server_;
};

}

#endif // !GRPC_SERVER_H
