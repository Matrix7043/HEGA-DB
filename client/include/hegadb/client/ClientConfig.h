#ifndef HEGADB_CLIENT_CLIENT_CONFIG_H
#define HEGADB_CLIENT_CLIENT_CONFIG_H

#include <string>

namespace hegadb::client {

class ClientConfig {
public:
    static ClientConfig fromArguments(
        int argc,
        char** argv);

    static void printHelp();
    static void printVersion();

    std::string const& serverAddress() const;

private:
    explicit ClientConfig(
        std::string server_address);

private:
    std::string server_address_;
};

}

#endif
