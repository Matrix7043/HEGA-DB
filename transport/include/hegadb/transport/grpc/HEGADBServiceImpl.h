#ifndef HEGADB_SERVICE_IMPL_H
#define HEGADB_SERVICE_IMPL_H

#include "hegadb/transport/ServerTransport.h"
#include "query.pb.h"
#include "response.pb.h"
#include "service.grpc.pb.h"

#include <functional>
#include <grpcpp/grpcpp.h>

namespace hegadb::transport::grpc {

class HEGADBServiceImpl final : public HEGADBService::Service {
public:
    using BatchWriter = ServerTransport::BatchWriter;
    using StreamingRequestHandler = ServerTransport::StreamingRequestHandler;
    using UnaryRequestHandler = ServerTransport::UnaryRequestHandler;
    using RequestHandler = UnaryRequestHandler;

    void setStreamingRequestHandler(StreamingRequestHandler handler);
    void setRequestHandler(RequestHandler handler);

    ::grpc::Status Execute(
        ::grpc::ServerContext* context,
        protocol::ExecuteQuery const* request,
        ::grpc::ServerWriter<protocol::QueryResponse>* writer) override;

private:
    StreamingRequestHandler streaming_handler_;
};

}

#endif
