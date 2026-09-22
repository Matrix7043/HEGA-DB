#ifndef HEGADB_CLIENT_CLIENT_H
#define HEGADB_CLIENT_CLIENT_H

#include "ClientConfig.h"

namespace hegadb::client {

class Client {
public:
    explicit Client(ClientConfig config);

    int run();

private:
    ClientConfig config_;
};

}

#endif
