#include <hegadb/client/ClientConfig.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace hegadb::client {

ClientConfig::ClientConfig(
    std::string server_address)
    : server_address_(std::move(server_address))
{
}

ClientConfig ClientConfig::fromArguments(
    int argc,
    char** argv)
{
    std::string server_address = "localhost:50051";

    for (int i = 1; i < argc; ++i) {

        std::string argument = argv[i];

        if (argument == "--help" || argument == "-h") {
            printHelp();
            std::exit(EXIT_SUCCESS);
        }

        if (argument == "--version" || argument == "-v") {
            printVersion();
            std::exit(EXIT_SUCCESS);
        }

        if (argument == "--server") {

            if (i + 1 >= argc) {
                throw std::invalid_argument(
                    "--server requires an address");
            }

            server_address = argv[++i];
            continue;
        }

        throw std::invalid_argument(
            "Unknown argument: " + argument);
    }

    return ClientConfig(std::move(server_address));
}

void ClientConfig::printHelp()
{
    std::cout
        << "HEGADB Client\n\n"
        << "Usage:\n"
        << "  hegadb_client [options]\n\n"
        << "Options:\n"
        << "  --server <address>  Server address "
           "(default: localhost:50051)\n"
        << "  --help, -h          Show this help message\n"
        << "  --version, -v       Show client version\n";
}

void ClientConfig::printVersion()
{
    std::cout
        << "HEGADB Client v0.7.0\n";
}

std::string const& ClientConfig::serverAddress() const
{
    return server_address_;
}

}
