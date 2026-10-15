#include "hegadb/transport/ServerTransportFactory.h"
#include <hegadb/logging/Logger.h>
#include <hegadb/logging/LoggingConfig.h>
#include <hegadb/server/ExecuteQueryReceiver.h>
#include <hegadb/server/QueryExecutor.h>
#include <hegadb/transport/ServerTransport.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <iostream>
#include <memory>
#include <sys/stat.h>

int main(int argc, char** argv)
{
    hegadb::logging::LoggingConfig logging_config;
    logging_config.console_enabled = true;
    logging_config.level = spdlog::level::info;
    hegadb::logging::initialize(logging_config);

    std::string server_address = "0.0.0.0:50051";

    hegadb::logging::info("Starting HEGA-DB Server on {}...", server_address);

    // Instantiate Query Receiver
    auto receiver = std::make_shared<hegadb::server::ExecuteQueryReceiver>();

    // Instantiate gRPC Transport Server
    auto server = hegadb::transport::ServerTransportFactory::createGrpc(server_address);

    server->setRequestHandler(
        [receiver](hegadb::protocol::ExecuteQuery const& request) {
            if (request.has_request_id())
                hegadb::logging::debug("Request ID: {}", request.request_id().SerializeAsString());
            return receiver->executeQuery(request);
        });

    server->start();
    hegadb::logging::info("HEGA-DB Server ready to receive ExecuteQuery requests.");

    server->wait();

    return 0;
}
