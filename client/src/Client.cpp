#include "hegadb/catalog/db-client/CatalogCache.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/protocol/ExecuteQueryEncoder.h"
#include "hegadb/crypto/CompositeEncryption.h"
#include "hegadb/logging/Logger.h"
#include "hegadb/transport/CatalogClientTransportFactory.h"
#include <hegadb/client/Client.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTPrinter.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/semantic/SemanticValidator.h>
#include <hegadb/crypto/MockEncryption.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/execution/ResultFormatter.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <hegadb/transport/ClientTransportFactory.h>
#include <hegadb/types/DataType.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>
#include <memory>
#include <string>
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

    auto encryption = std::make_shared<hegadb::crypto::CompositeEncryption>();

    hegadb::client::preparation::QueryPreparer preparer(
        *encryption);

    // ==========================================================
    // Catalog Client Transport
    // ==========================================================

    logging::debug("Creating gRPC catalog client transport");
    logging::debug("Connecting to catalogServiceAddress {}", config_.catalogServiceAddress());
    auto catalog_transport = hegadb::transport::CatalogClientTransportFactory::createGrpc(config_.catalogServiceAddress());

    logging::debug("Creating catalog cache");
    auto catalog_cache = hegadb::catalog::CatalogCache(*catalog_transport);
    catalog_cache.initialize();

    hegadb::client::semantic::SemanticValidator validator(catalog_cache.catalog());

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
            auto parseResult = sqlParser.parse(input);
            std::cout << "Query rejected (syntax error):\n";
            for (auto const& err : parseResult.errors) {
                std::cout << "  " << err << '\n';
            }
            if (parseResult.errors.empty()) {
                std::cout << "  Unable to parse SQL statement.\n";
            }
            continue;
        }

        logging::debug("SQL Parsing completed successfully");

        // ========================================================
        // Semantic validation
        // ========================================================

        logging::debug("Started semantic validation");
        try {
            bool refresh_status = catalog_cache.refreshIfNeeded();

            logging::debug("Catalog Refersh Check New version found: {}", refresh_status);

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

        // SELECT continues below; CREATE TABLE / DROP TABLE take the DDL path.
        auto const* select_stmt = dynamic_cast<ast::SelectStmt const*>(ast.get());

        if (select_stmt == nullptr) {
            // CREATE TABLE / DROP TABLE: no encryption or query preparation,
            // the validated statement is sent to the server as it is.
            try {
                logging::debug(
                    "Encoding DDL request: query_id={}",
                    queryId);

                auto request = hegadb::client::protocol::ExecuteQueryEncoder::encodeDdl(
                    *ast, queryId);

                std::cout << "Sending statement to server...\n";

                auto response = transport->execute(request);

                logging::debug(
                    "Received DDL response: query_id={} status={}",
                    queryId,
                    static_cast<int>(response.status().code()));

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
                    << "Statement execution failed: "
                    << error.what()
                    << '\n';

                logging::error(
                    "DDL execution failed: query_id={} error={}",
                    queryId,
                    error.what());
            }

            continue;
        }

        std::cout << "Query accepted.\n";

        hegadb::client::ast::ASTPrinter printer;
        auto const astPath = printer.print(select_stmt,
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

        // Save table names and mask kind before AST is moved
        std::string from_table = (select_stmt->from() != nullptr) ? select_stmt->from()->name() : "";
        std::string join_table = (select_stmt->join() != nullptr && select_stmt->join()->table() != nullptr)
            ? select_stmt->join()->table()->name()
            : "";

        hegadb::server::execution::MaskKind mask_kind = hegadb::server::execution::MaskKind::DIFF_NONZERO;
        if (select_stmt->where()) {
            if (auto const* comp = dynamic_cast<ast::ComparisonExpr const*>(select_stmt->where())) {
                switch (comp->op()) {
                    case operations::ComparisonOp::EQ:
                    case operations::ComparisonOp::NEQ:
                    case operations::ComparisonOp::LT:
                    case operations::ComparisonOp::GT:
                    case operations::ComparisonOp::LTE:
                    case operations::ComparisonOp::GTE:
                        mask_kind = hegadb::server::execution::MaskKind::DIFF_NONZERO;
                        break;
                }
            } else {
                mask_kind = hegadb::server::execution::MaskKind::COMPOSED;
            }
        }

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

            if (response.status().code() == hegadb::protocol::STATUS_OK && response.has_result()) {
                auto const& rp = response.result();
                if (rp.columns_size() > 0 && rp.row_count() > 0) {
                    hegadb::server::execution::ExecutionResult exec_result;
                    exec_result.setResultMode(rp.mode());

                    if (!rp.encrypted_mask().empty()) {
                        auto mask_ct = crypto::OpenFHECiphertext::deserialize(rp.encrypted_mask());
                        hegadb::server::execution::EncryptedMask mask(
                            std::move(mask_ct),
                            rp.row_count(),
                            mask_kind
                        );
                        exec_result.setEncryptedMask(std::move(mask));
                    }

                    hegadb::server::execution::ExecutionBatch batch;
                    for (auto const& col_proto : rp.columns()) {
                        for (auto const& serialised_ct : col_proto.ciphertexts()) {
                            auto ct = crypto::OpenFHECiphertext::deserialize(serialised_ct);
                            batch.addColumn(col_proto.column_name(), std::move(ct));
                        }
                    }
                    batch.setRowCount(rp.row_count());
                    exec_result.addBatch(std::move(batch));

                    auto findColumnType = [&](std::string const& col_name) -> types::DataType {
                        auto dot = col_name.find('.');
                        if (dot != std::string::npos) {
                            auto tbl = col_name.substr(0, dot);
                            auto col = col_name.substr(dot + 1);
                            auto const* tschema = catalog_cache.catalog().findTable(tbl);
                            if (tschema) {
                                auto const* cs = tschema->findColumn(col);
                                if (cs) return cs->type;
                            }
                        }
                        if (!from_table.empty()) {
                            auto const* tschema = catalog_cache.catalog().findTable(from_table);
                            if (tschema) {
                                auto const* cs = tschema->findColumn(col_name);
                                if (cs) return cs->type;
                            }
                        }
                        if (!join_table.empty()) {
                            auto const* tschema = catalog_cache.catalog().findTable(join_table);
                            if (tschema) {
                                auto const* cs = tschema->findColumn(col_name);
                                if (cs) return cs->type;
                            }
                        }
                        return types::DataType::Integer;
                    };

                    std::vector<std::string> col_names;
                    std::vector<types::DataType> col_types;
                    for (auto const& col_proto : rp.columns()) {
                        col_names.push_back(col_proto.column_name());
                        col_types.push_back(findColumnType(col_proto.column_name()));
                    }

                    hegadb::execution::ResultDecryptor decryptor(encryption);
                    auto decrypted = decryptor.decrypt(exec_result, col_names, col_types);

                    hegadb::execution::FormatConfig fmt;
                    fmt.format = hegadb::execution::DisplayFormat::Table;
                    hegadb::execution::ResultFormatter formatter(fmt);
                    formatter.format(decrypted, std::cout);
                    std::cout << '\n';
                } else {
                    std::cout << "(0 rows)\n\n";
                }
            }

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
