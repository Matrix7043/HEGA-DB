#ifndef CLIENT_TRANSPORT_FACTORY_H
#define CLIENT_TRANSPORT_FACTORY_H

#include "hegadb/transport/ClientTransport.h"
#include <memory>
namespace hegadb::transport {

class ClientTransportFactory {
public:
    static std::unique_ptr<ClientTransport> createGrpc(
        std::string const& addr);
};

}

#endif // !CLIENT_TRANSPORT_FACTORY_H
