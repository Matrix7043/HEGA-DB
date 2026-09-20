#include <GrpcServer.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/server/ExecuteQueryReceiver.h>
#include <hegadb/server/QueryExecutor.h>
#include <hegadb/transport/ServerTransport.h>

#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
    std::string server_address = "0.0.0.0:50051";

    std::cout << "Starting HEGA-DB Server on " << server_address << "...\n";

    // Instantiate Query Receiver
    auto receiver = std::make_shared<hegadb::server::ExecuteQueryReceiver>();

    // Instantiate gRPC Transport Server
    // TODO : Make Server Transport the only usable interface
    hegadb::transport::grpc::GrpcServer server(server_address);

    server.setRequestHandler(
        [receiver](hegadb::protocol::ExecuteQuery const& request) {
            return receiver->executeQuery(request);
        });

    server.start();
    std::cout << "HEGA-DB Server ready to receive ExecuteQuery requests.\n";

    server.wait();

    return 0;
}
