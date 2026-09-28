#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/protocol/ExecuteQueryEncoder.h"
#include "hegadb/logging/Logger.h"
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

    logging::debug("Creating gRPC client transport");
    logging::debug("Connecting to serverAddress {}", config_.serverAddress());
    auto transport = hegadb::transport::ClientTransportFactory::createGrpc(config_.serverAddress());

    // ==========================================================
    // Readline
    // ==========================================================

    int count { 0 };

    using_history();

    while (true) {
        char* raw_input { readline("hegadb> ") };

        auto const queryId = "client-request-" + std::to_string(count++);

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
            logging::debug("SQL Parsing failed");
            std::cout << "Query rejected.\n";
            continue;
        }

        logging::debug("SQL Parsing completed successfully");

        // ========================================================
        // Semantic validation
        // ========================================================

        logging::debug("Started semantic validation");
        try {
            validator.validate(*ast);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            logging::warn(
                "Semantic validation rejected query: {}",
                error.what());
            std::cout
                << "Semantic error: "
                << error.what()
                << '\n';

            continue;
        }
        // ========================================================
        // Query accepted
        // ========================================================

        logging::debug(
            "Semantic validation completed successfully");

        std::cout << "Query accepted.\n";

        hegadb::client::ast::ASTPrinter printer;
        auto const astPath = printer.print(dynamic_cast<ast::SelectStmt const*>(ast.get()),
            "logs/ast", queryId);
        if (!astPath.empty()) {
            logging::debug(
                "AST written to: {}",
                astPath.string());
        } else {
            logging::warn(
                "Failed to write AST: query_id={}",
                queryId);
        }

        // ========================================================
        // Query preparation
        // ========================================================

        try {
            logging::debug(
                "Preparing query for encrypted execution");
            auto prepared = preparer.prepare(std::move(ast));

            std::cout
                << "Preparing query for encrypted execution...\n";

            logging::debug(
                "Query preparation completed: encrypted_parameters={}",
                prepared.parameters().size());

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
                auto const astpPath = printer.print(preparedSelect, "logs/ast-p", queryId);
                if (!astpPath.empty()) {
                    logging::debug(
                        "Parameterized AST written to: {}",
                        astpPath.string());
                } else {
                    logging::warn(
                        "Failed to write parameterized AST: query_id={}",
                        queryId);
                }
            }

            std::cout
                << "Query preparation successful.\n";

            // ====================================================
            // Encode request
            // ====================================================
            logging::debug(
                "Encoding query request: query_id={}",
                queryId);

            auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
                prepared, queryId);

            logging::debug(
                "Sending query: query_id={}",
                queryId);

            std::cout
                << "Sending query to server...\n";

            // ====================================================
            // Execute remotely
            // ====================================================

            auto response = transport->execute(request);

            logging::debug(
                "Received query response: query_id={} status={}",
                response.request_id().SerializeAsString(),
                response.status().SerializeAsString());

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

            logging::error(
                "Query execution failed: query_id={} error={}",
                queryId,
                error.what());

            continue;
        }
    }

    logging::info(
        "HEGADB client shutting down");

    return 0;
}

}
