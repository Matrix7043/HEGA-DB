#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/TableRef.h>
#include <hegadb/client/ast/Wildcard.h>

#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/crypto/MockCiphertext.h>
#include <hegadb/crypto/MockEncryption.h>

#include "common.pb.h"
#include "query.pb.h"

// Protobuf utility for human-readable text printing
#include <google/protobuf/text_format.h>

#include <cassert>
#include <iostream>
#include <memory>

using namespace hegadb::protocol;

// Helper to map operations::ComparisonOp to Proto ComparisonOp
ComparisonOp mapComparisonOp(hegadb::operations::ComparisonOp op)
{
    switch (op) {
    case hegadb::operations::ComparisonOp::EQ:
        return ComparisonOp::EQ;
    case hegadb::operations::ComparisonOp::NEQ:
        return ComparisonOp::NEQ;
    case hegadb::operations::ComparisonOp::LT:
        return ComparisonOp::LT;
    case hegadb::operations::ComparisonOp::GT:
        return ComparisonOp::GT;
    case hegadb::operations::ComparisonOp::LTE:
        return ComparisonOp::LTE;
    case hegadb::operations::ComparisonOp::GTE:
        return ComparisonOp::GTE;
    }
    return ComparisonOp::EQ;
}

// Convert C++ AST expression to Proto expression
void convertExpression(hegadb::client::ast::Expr const* node, Expression* proto_expr)
{
    if (!node)
        return;

    if (auto const* col = dynamic_cast<hegadb::client::ast::ColumnRef const*>(node)) {
        proto_expr->mutable_column_ref()->set_column(col->name());
    } else if (auto const* param = dynamic_cast<hegadb::client::ast::ParameterRef const*>(node)) {
        proto_expr->mutable_parameter_ref()->set_parameter_index(param->parameterIndex());
    } else if (dynamic_cast<hegadb::client::ast::Wildcard const*>(node)) {
        proto_expr->mutable_wildcard();
    } else if (auto const* comp = dynamic_cast<hegadb::client::ast::ComparisonExpr const*>(node)) {
        auto* proto_comp = proto_expr->mutable_comparison_expr();
        convertExpression(comp->left(), proto_comp->mutable_left());
        proto_comp->set_operator_(mapComparisonOp(comp->op()));
        convertExpression(comp->right(), proto_comp->mutable_right());
    }
}

// Convert PreparedQuery to Proto ExecuteQuery message
ExecuteQuery toProto(hegadb::client::preparation::PreparedQuery const& prepared)
{
    ExecuteQuery proto_req;
    proto_req.set_query_id(1);

    auto const* select_ast = dynamic_cast<hegadb::client::ast::SelectStmt const*>(&prepared.query());
    assert(select_ast != nullptr);

    auto* proto_select = proto_req.mutable_statement()->mutable_select_statement();

    // Set FROM table
    if (select_ast->from()) {
        proto_select->mutable_from_table()->set_table(select_ast->from()->name());
    }

    // Set Projections
    for (auto const& proj : select_ast->projections()) {
        convertExpression(proj.get(), proto_select->add_projections());
    }

    // Set WHERE clause
    if (select_ast->where()) {
        convertExpression(select_ast->where(), proto_select->mutable_where());
    }

    // Set Encrypted Parameters
    for (auto const& p : prepared.parameters()) {
        auto* proto_p = proto_req.add_parameters();
        proto_p->set_index(p.index());
        proto_p->set_ciphertext("MOCK_CIPHERTEXT_BYTES");
    }

    return proto_req;
}

int main()
{
    std::cout << "--- Starting Server Protocol Integration Test ---" << std::endl;

    // 1. Client: Parse SQL using SQLParser and QueryPreparer
    hegadb::client::SQLParser parser;
    hegadb::crypto::MockEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    std::string sql = "SELECT * FROM STUDENTS WHERE GRADE > 90;";
    auto ast = parser.parseToAST(sql);
    assert(ast != nullptr && "Failed to parse SQL");

    auto prepared = preparer.prepare(std::move(ast));
    std::cout << "[Client] Parsed SQL query: " << sql << std::endl;

    // 2. Client: Convert PreparedQuery to Protobuf
    ExecuteQuery proto_query = toProto(prepared);

    // -------------------------------------------------------------
    // Print Human-Readable Protobuf Format
    // -------------------------------------------------------------
    std::string text_format_output;
    google::protobuf::TextFormat::PrintToString(proto_query, &text_format_output);
    std::cout << "\n================ HUMAN-READABLE PROTOBUF ================\n"
              << text_format_output
              << "=========================================================\n"
              << std::endl;

    // 3. Serialize to binary byte stream
    std::string binary_payload;
    bool serialized = proto_query.SerializeToString(&binary_payload);
    assert(serialized && "Serialization failed");
    std::cout << "[Client] Serialized query into binary (" << binary_payload.size() << " bytes)" << std::endl;

    // 4. Server: Receive binary payload and parse
    ExecuteQuery server_received;
    bool parsed = server_received.ParseFromString(binary_payload);
    assert(parsed && "Server failed to parse payload");
    std::cout << "[Server] Successfully parsed binary payload" << std::endl;

    // 5. Server: Assertions
    assert(server_received.statement().has_select_statement());
    auto const& select = server_received.statement().select_statement();
    assert(select.from_table().table() == "STUDENTS");
    assert(select.where().has_comparison_expr());
    assert(select.where().comparison_expr().operator_() == ComparisonOp::GT);
    assert(server_received.parameters_size() == 1);

    std::cout << "\n--- SERVER PROTOCOL INTEGRATION TEST PASSED! ---" << std::endl;
    return 0;
}
