#include "hegadb/client/protocol/ExecuteQueryEncoder.h"
#include <hegadb/client/Client.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTPrinter.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/crypto/MockEncryption.h>
#include <hegadb/transport/ClientTransportFactory.h>
#include <hegadb/types/DataType.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hegadb::client {

Client::Client(ClientConfig config)
    : config_(std::move(config))
{
}

int Client::run()
{
    std::cout << "HEGADB Client v0.7\n";
    std::cout << "Type SQL queries. Type \\q to exit.\n\n";

    // ==========================================================
    // Client Catalog
    // ==========================================================

    std::vector<hegadb::catalog::ColumnSchema> studentColumns = {
        { "NAME",
            hegadb::types::DataType::String,
            false,
            0 },
        { "CLASS",
            hegadb::types::DataType::Integer,
            false,
            1 },
        { "GRADE",
            hegadb::types::DataType::Integer,
            false,
            2 },
    };

    hegadb::catalog::TableSchema students(
        "STUDENTS",
        std::move(studentColumns));

    std::unordered_map<
        std::string,
        hegadb::catalog::TableSchema>
        tables;

    tables.emplace(
        "STUDENTS",
        std::move(students));

    auto snapshot = std::make_shared<
        hegadb::catalog::CatalogSnapshot const>(
        1,
        std::move(tables));

    hegadb::catalog::InMemoryCatalog catalog(snapshot);

    hegadb::client::semantic::SemanticValidator validator(
        catalog);

    // ==========================================================
    // Query Preparation
    // ==========================================================

    hegadb::crypto::MockEncryption encryption;

    hegadb::client::preparation::QueryPreparer preparer(
        encryption);

    // ==========================================================
    // Client Transport
    // ==========================================================

    auto transport = hegadb::transport::ClientTransportFactory::createGrpc(
        "localhost:50051");

    // ==========================================================
    // Readline
    // ==========================================================

    int count { 0 };

    using_history();

    while (true) {
        char* raw_input { readline("hegadb> ") };

        if (raw_input == nullptr) {
            std::cout << '\n';
            break;
        }

        std::string input(raw_input);

        free(raw_input);

        if (input.empty()) {
            continue;
        }

        if (input == "\\q") {
            break;
        }

        add_history(input.c_str());

        // ========================================================
        // Parse
        // ========================================================

        hegadb::client::SQLParser sqlParser;

        auto ast { sqlParser.parseToAST(input) };

        if (!ast) {
            std::cout << "Query rejected.\n";
            continue;
        }

        // ========================================================
        // Semantic validation
        // ========================================================

        try {
            validator.validate(*ast);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            std::cout
                << "Semantic error: "
                << error.what()
                << '\n';

            continue;
        }

        // ========================================================
        // Query accepted
        // ========================================================

        std::cout << "Query accepted.\n";

        std::cout << "AST:\n";

        hegadb::client::ast::ASTPrinter printer;

        printer.print(ast.get());

        // ========================================================
        // Query preparation
        // ========================================================

        try {
            auto prepared = preparer.prepare(std::move(ast));

            std::cout
                << "Preparing query for encrypted execution...\n";

            std::cout
                << "Encrypted parameters: "
                << prepared.parameters().size()
                << '\n';

            for (auto const& parameter :
                prepared.parameters()) {

                std::cout
                    << "  Parameter "
                    << parameter.index()
                    << ": encrypted\n";
            }

            std::cout
                << "Parameterized AST:\n";

            auto const* preparedSelect = dynamic_cast<
                hegadb::client::ast::SelectStmt const*>(
                &prepared.query());

            if (preparedSelect != nullptr) {
                printer.print(preparedSelect);
            }

            std::cout
                << "Query preparation successful.\n";

            // ====================================================
            // Encode request
            // ====================================================

            auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
                prepared,
                "client-request-" + std::to_string(count++));

            std::cout
                << "Sending query to server...\n";

            // ====================================================
            // Execute remotely
            // ====================================================

            auto response = transport->execute(request);

            std::cout
                << "Server response:\n"
                << "  Request ID: "
                << response.request_id().value()
                << '\n'
                << "  Status: "
                << response.status().code()
                << '\n'
                << "  Message: "
                << response.status().message()
                << "\n\n";

        } catch (std::exception const& error) {

            std::cout
                << "Query execution failed: "
                << error.what()
                << '\n';

            continue;
        }
    }

    return 0;
}

}
