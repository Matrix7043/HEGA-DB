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
    std::string const& catalogServiceAddress() const;
    std::string const& logFile() const;
    std::string const& logLevel() const;

private:
    explicit ClientConfig(
        std::string server_address,
        std::string catalog_service_address,
        std::string log_file,
        std::string log_level);

private:
    std::string server_address_;
    std::string catalog_service_address_;
    std::string log_file_;
    std::string log_level_;
};

}

#endif
