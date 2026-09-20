#ifndef SERVER_TRANSPORT_FACTORY_H
#define SERVER_TRANSPORT_FACTORY_H

#include "hegadb/transport/ServerTransport.h"
#include <memory>

namespace hegadb::transport {

class ServerTransportFactory {

public:
    static std::unique_ptr<ServerTransport> createGrpc(
        std::string const& addr);
};

}

#endif // !SERVER_TRANSPORT_FACTORY_H
