#ifndef SERVER_TRANSPORT_H
#define SERVER_TRANSPORT_H

#include "query.pb.h"
#include <functional>
namespace hegadb::transport {

class ServerTransport {
public:
    using RequestHandler = std::function<protocol::QueryResponse(protocol::ExecuteQuery const&)>;

    virtual ~ServerTransport() = default;

    virtual void setRequestHandler(RequestHandler handler) = 0;

    virtual void start() = 0;
    virtual void wait() = 0;
    virtual void shutdown() = 0;
};

}

#endif
