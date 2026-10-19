#ifndef SERVER_TRANSPORT_H
#define SERVER_TRANSPORT_H

#include "query.pb.h"
#include "response.pb.h"
#include <functional>
namespace hegadb::transport {

class ServerTransport {
public:
    using BatchWriter = std::function<bool(protocol::QueryResponse const&)>;
    using StreamingRequestHandler = std::function<void(protocol::ExecuteQuery const&, BatchWriter)>;
    using UnaryRequestHandler = std::function<protocol::QueryResponse(protocol::ExecuteQuery const&)>;
    using RequestHandler = UnaryRequestHandler;

    virtual ~ServerTransport() = default;

    virtual void setStreamingRequestHandler(StreamingRequestHandler handler) = 0;
    virtual void setRequestHandler(RequestHandler handler) = 0;

    virtual void start() = 0;
    virtual void wait() = 0;
    virtual void shutdown() = 0;
};

}

#endif
