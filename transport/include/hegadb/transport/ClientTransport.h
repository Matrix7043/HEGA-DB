#ifndef CLIENT_TRANSPORT_H
#define CLIENT_TRANSPORT_H

#include "query.pb.h"
#include "response.pb.h"
#include <functional>

namespace hegadb::transport {

class ClientTransport {
public:
    using BatchCallback = std::function<bool(protocol::QueryResponse const&)>;

    virtual ~ClientTransport() = default;

    virtual protocol::QueryResponse execute(
        protocol::ExecuteQuery const& request) = 0;

    virtual protocol::Status executeStream(
        protocol::ExecuteQuery const& request,
        BatchCallback callback) = 0;
};

}

#endif // !CLIENT_TRANSPORT_H
