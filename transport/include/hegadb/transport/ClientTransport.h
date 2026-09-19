#ifndef CLIENT_TRANSPORT_H
#define CLIENT_TRANSPORT_H

#include "query.pb.h"
namespace hegadb::transport {

class ClientTransport {
public:
    virtual ~ClientTransport() = default;

    virtual protocol::QueryResponse execute(
        protocol::ExecuteQuery const& request) = 0;
};

}

#endif // !CLIENT_TRANSPORT_H
