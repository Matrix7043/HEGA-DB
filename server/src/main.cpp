#include "hegadb/transport/ServerTransportFactory.h"
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/server/ExecuteQueryReceiver.h>
#include <hegadb/server/QueryExecutor.h>
#include <hegadb/transport/ServerTransport.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <iostream>
#include <memory>
#include <sys/stat.h>

int main(int argc, char** argv)
{
    std::string server_address = "0.0.0.0:50051";

    std::cout << "Starting HEGA-DB Server on " << server_address << "...\n";

    // Instantiate Query Receiver
    auto receiver = std::make_shared<hegadb::server::ExecuteQueryReceiver>();

    // Instantiate gRPC Transport Server
    auto server = hegadb::transport::ServerTransportFactory::createGrpc(server_address);

    server->setRequestHandler(
        [receiver](hegadb::protocol::ExecuteQuery const& request) {
            if (request.has_request_id())
                std::cout << request.request_id().SerializeAsString() << std::endl;
            return receiver->executeQuery(request);
        });

    server->start();
    std::cout << "HEGA-DB Server ready to receive ExecuteQuery requests.\n";

    server->wait();

    return 0;
}
