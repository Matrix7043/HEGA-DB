#include "hegadb/logging/LoggingConfig.h"
#include <exception>
#include <hegadb/client/Client.h>
#include <hegadb/client/ClientConfig.h>
#include <hegadb/logging/Logger.h>
#include <iostream>
#include <spdlog/common.h>

int main(int argc, char** argv)
{

    try {

        hegadb::logging::LoggingConfig logging_config;

        auto config = hegadb::client::ClientConfig::fromArguments(
            argc,
            argv);

        logging_config.level = hegadb::logging::logLevelEnumParser(config.logLevel());
        logging_config.console_enabled = true;
        logging_config.file_enabled = !config.logFile().empty();
        if (logging_config.file_enabled)
            logging_config.file_path = config.logFile();

        hegadb::logging::initialize(logging_config);

        hegadb::logging::info("HEGADB client starting");
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
