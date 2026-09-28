#include <exception>
#include <hegadb/client/Client.h>
#include <hegadb/client/ClientConfig.h>
#include <hegadb/logging/Logger.h>
#include <iostream>

int main(int argc, char** argv)
{

    try {

        hegadb::logging::initialize();

        hegadb::logging::info("HEGADB client starting");

        hegadb::logging::debug("Arguments recieved: {}", argc);

        auto config = hegadb::client::ClientConfig::fromArguments(
            argc,
            argv);

        hegadb::client::Client client(
            std::move(config));

        return client.run();

    } catch (
        std::exception const& error) {

        std::cerr
            << "HEGADB client error: "
            << error.what()
            << '\n';

        return 1;
    }
}
