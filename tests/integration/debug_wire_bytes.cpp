/**
 * debug_wire_bytes.cpp  — one-shot diagnostic
 *
 * Runs a single SELECT id FROM EMPLOYEES query over gRPC, then prints
 * the first 20 bytes (hex + ASCII) of the first column's first ciphertext
 * blob as received from the wire, so we can confirm the HGCT magic header.
 */
#include <hegadb/transport/grpc/GrpcClient.h>
#include <hegadb/transport/grpc/GrpcServer.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/types/DataType.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>

using namespace hegadb;

int main()
{
    std::string home     = std::getenv("HOME");
    std::string data_dir    = home + "/.hegadb/data";
    std::string keys_dir    = home + "/.hegadb/keys";
    std::string catalog_dir = home + "/.hegadb/catalog";

    if (!std::filesystem::exists(keys_dir)) {
        std::cerr << "Keys not found at " << keys_dir << "\n";
        return 1;
    }

    auto encryption     = std::make_shared<crypto::CompositeEncryption>(keys_dir);
    auto crypto_context = std::make_shared<crypto::OpenFHEContext>();
    crypto_context->setBFVContext(encryption->getContext().getBFVContext());
    crypto_context->setCKKSContext(encryption->getContext().getCKKSContext());

    std::vector<catalog::ColumnSchema> cols = {
        {"id",     types::DataType::Integer, false, 0},
        {"age",    types::DataType::Integer, false, 1},
        {"salary", types::DataType::Integer, false, 2},
        {"dept",   types::DataType::String,  false, 3},
    };
    catalog::TableSchema schema("EMPLOYEES", std::move(cols));
    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("EMPLOYEES", std::move(schema));
    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables));
    auto catalog  = std::make_shared<catalog::InMemoryCatalog>(snapshot);
    std::shared_ptr<server::storage::StorageManager> storage{
        server::storage::StorageManager::createFileStorage(data_dir, catalog_dir)};

    auto executor = std::make_shared<server::QueryExecutor>(catalog, storage, crypto_context);
    auto receiver = std::make_shared<server::ExecuteQueryReceiver>(catalog, executor);

    auto server = std::make_unique<transport::grpc::GrpcServer>("127.0.0.1:0");
    server->setRequestHandler(
        [receiver](protocol::ExecuteQuery const& req) {
            return receiver->executeQuery(req);
        });
    server->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    std::string addr = server->address();

    // Build a minimal SELECT id FROM EMPLOYEES request
    client::SQLParser parser;
    auto ast = parser.parseToAST("SELECT id FROM EMPLOYEES");
    client::preparation::QueryPreparer preparer(*encryption);
    auto prepared = preparer.prepare(std::move(ast));
    auto request  = client::protocol::ExecuteQueryEncoder::encode(prepared, "debug-001");

    transport::grpc::GrpcClient client(addr);
    auto response = client.execute(request);

    auto const& rp = response.result();
    std::cout << "status=" << response.status().code()
              << " row_count=" << rp.row_count()
              << " columns=" << rp.columns_size() << "\n";

    if (rp.columns_size() > 0) {
        auto const& col = rp.columns(0);
        std::cout << "col name=" << col.column_name()
                  << " ciphertexts=" << col.ciphertexts_size() << "\n";

        if (col.ciphertexts_size() > 0) {
            auto const& blob = col.ciphertexts(0);
            std::cout << "blob size=" << blob.size() << " bytes\n";
            std::cout << "first 20 bytes (hex): ";
            for (size_t i = 0; i < std::min<size_t>(20, blob.size()); ++i)
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << (static_cast<unsigned>(blob[i]) & 0xFF) << " ";
            std::cout << std::dec << "\n";
            std::cout << "first 4 bytes (ASCII): "
                      << blob[0] << blob[1] << blob[2] << blob[3] << "\n";

            // Try to deserialize
            try {
                auto ct = crypto::OpenFHECiphertext::deserialize(blob);
                std::cout << "deserialize OK, scheme=" << static_cast<int>(ct.getSchemeType())
                          << " type=" << static_cast<int>(ct.getOriginalType()) << "\n";
            } catch (std::exception const& e) {
                std::cout << "deserialize FAILED: " << e.what() << "\n";
            }
        }
    }

    server->shutdown();
    return 0;
}
