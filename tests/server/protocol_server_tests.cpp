#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/NotExpr.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/TableRef.h>
#include <hegadb/client/ast/Wildcard.h>

#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/crypto/MockCiphertext.h>
#include <hegadb/crypto/MockEncryption.h>

#include "common.pb.h"
#include "query.pb.h"

#include <google/protobuf/text_format.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ast = hegadb::client::ast;
namespace proto = hegadb::protocol;
namespace ops = hegadb::operations;

struct TestCase {
    std::string name;
    std::string sql;
    bool expect_success;
};

// Helper: Map ops::ComparisonOp to proto::ComparisonOp
proto::ComparisonOp mapComparisonOp(ops::ComparisonOp op)
{
    switch (op) {
    case ops::ComparisonOp::EQ:
        return proto::ComparisonOp::EQ;
    case ops::ComparisonOp::NEQ:
        return proto::ComparisonOp::NEQ;
    case ops::ComparisonOp::LT:
        return proto::ComparisonOp::LT;
    case ops::ComparisonOp::GT:
        return proto::ComparisonOp::GT;
    case ops::ComparisonOp::LTE:
        return proto::ComparisonOp::LTE;
    case ops::ComparisonOp::GTE:
        return proto::ComparisonOp::GTE;
    }
    return proto::ComparisonOp::EQ;
}

// Helper: Map ops::LogicalOp to proto::LogicalOp
proto::LogicalOp mapLogicalOp(ops::LogicalOp op)
{
    switch (op) {
    case ops::LogicalOp::AND:
        return proto::LogicalOp::AND;
    case ops::LogicalOp::OR:
        return proto::LogicalOp::OR;
    }
    return proto::LogicalOp::AND;
}

// Recursive Converter: C++ AST Expression -> Protobuf Expression
void convertExpression(ast::Expr const* node, proto::Expression* proto_expr)
{
    if (!node)
        return;

    if (auto const* col = dynamic_cast<ast::ColumnRef const*>(node)) {
        proto_expr->mutable_column_ref()->set_column(col->name());
    } else if (auto const* param = dynamic_cast<ast::ParameterRef const*>(node)) {
        proto_expr->mutable_parameter_ref()->set_parameter_index(param->parameterIndex());
    } else if (dynamic_cast<ast::Wildcard const*>(node)) {
        proto_expr->mutable_wildcard();
    } else if (auto const* comp = dynamic_cast<ast::ComparisonExpr const*>(node)) {
        auto* proto_comp = proto_expr->mutable_comparison_expr();
        convertExpression(comp->left(), proto_comp->mutable_left());
        proto_comp->set_operator_(mapComparisonOp(comp->op()));
        convertExpression(comp->right(), proto_comp->mutable_right());
    } else if (auto const* logical = dynamic_cast<ast::LogicalExpr const*>(node)) {
        auto* proto_logical = proto_expr->mutable_logical_expr();
        convertExpression(logical->left(), proto_logical->mutable_left());
        proto_logical->set_operator_(mapLogicalOp(logical->op()));
        convertExpression(logical->right(), proto_logical->mutable_right());
    } else if (auto const* not_expr = dynamic_cast<ast::NotExpr const*>(node)) {
        auto* proto_not = proto_expr->mutable_not_expr();
        convertExpression(not_expr->child(), proto_not->mutable_child());
    }
}

// Converter: PreparedQuery -> Protobuf ExecuteQuery Message
proto::ExecuteQuery toProto(hegadb::client::preparation::PreparedQuery const& prepared, uint32_t query_id)
{
    proto::ExecuteQuery proto_req;
    proto_req.set_query_id(query_id);

    auto const* select_ast = dynamic_cast<ast::SelectStmt const*>(&prepared.query());
    assert(select_ast != nullptr);

    auto* proto_select = proto_req.mutable_statement()->mutable_select_statement();

    // 1. FROM Table
    if (select_ast->from()) {
        proto_select->mutable_from_table()->set_table(select_ast->from()->name());
    }

    // 2. Projections
    for (auto const& proj : select_ast->projections()) {
        convertExpression(proj.get(), proto_select->add_projections());
    }

    // 3. WHERE Clause (handles logical AND, OR, NOT, Comparison, and Parentheses)
    if (select_ast->where()) {
        convertExpression(select_ast->where(), proto_select->mutable_where());
    }

    // 4. Encrypted Parameters
    for (auto const& p : prepared.parameters()) {
        auto* proto_p = proto_req.add_parameters();
        proto_p->set_index(p.index());
        proto_p->set_ciphertext("MOCK_ENCRYPTED_BYTES");
    }

    return proto_req;
}

int main()
{
    std::cout << "=========================================================\n";
    std::cout << "     HEGA-DB PROTOCOL SUITE: MULTI-QUERY TEST SUITE     \n";
    std::cout << "=========================================================\n\n";

    hegadb::client::SQLParser parser;
    hegadb::crypto::MockEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    std::vector<TestCase> const tests = {
        { "Basic SELECT *", "SELECT * FROM employees;", true },
        { "Single column", "SELECT name FROM employees;", true },
        { "Multiple columns", "SELECT name, salary, department_id FROM employees;", true },
        { "WHERE comparison", "SELECT name FROM employees WHERE salary > 50000;", true },
        { "WHERE string", "SELECT name FROM employees WHERE name = 'Alice';", true },
        { "WHERE AND", "SELECT name FROM employees WHERE salary > 50000 AND department_id = 10;", true },
        { "WHERE OR", "SELECT name FROM employees WHERE salary > 50000 OR department_id = 10;", true },
        { "WHERE NOT", "SELECT name FROM employees WHERE NOT salary > 50000;", true },
        { "WHERE parentheses", "SELECT name FROM employees WHERE (salary > 50000 OR department_id = 10) AND active = TRUE;", true },
        { "Missing SELECT list", "SELECT FROM employees;", false },
        { "Missing FROM", "SELECT name;", false },
        { "Missing table", "SELECT name FROM;", false },
        { "Missing WHERE expression", "SELECT name FROM employees WHERE;", false },
        { "Unsupported ORDER BY", "SELECT name FROM employees ORDER BY name;", false },
        { "Unsupported arithmetic", "SELECT salary + 100 FROM employees;", false },
        { "Unsupported JOIN", "SELECT * FROM employees JOIN departments;", false }
    };

    uint32_t query_id = 1;
    size_t passed_count = 0;

    for (auto const& test : tests) {
        std::cout << "---------------------------------------------------------\n";
        std::cout << "[Test #" << query_id << "] " << test.name << "\n";
        std::cout << "SQL: \"" << test.sql << "\"\n";

        auto ast = parser.parseToAST(test.sql);

        if (!test.expect_success) {
            // Negative test case: parsing MUST fail
            assert(ast == nullptr && "Expected SQL parsing to fail for unsupported/invalid SQL!");
            std::cout << "[RESULT] PASS (Syntax/Feature correctly rejected by parser)\n";
            passed_count++;
            query_id++;
            continue;
        }

        // Positive test case: parsing MUST succeed
        assert(ast != nullptr && "Expected SQL parsing to succeed!");

        // 1. Prepare query (extract & parameterize literals)
        auto prepared = preparer.prepare(std::move(ast));

        // 2. Convert to Protobuf
        proto::ExecuteQuery proto_query = toProto(prepared, query_id);

        // 3. Print Human-Readable Protobuf Representation
        std::string text_output;
        google::protobuf::TextFormat::PrintToString(proto_query, &text_output);
        std::cout << "\n--- PROTOBUF REPRESENTATION ---\n"
                  << text_output;

        // 4. Binary Serialization & Server Deserialization Verification
        std::string binary_payload;
        bool serialized = proto_query.SerializeToString(&binary_payload);
        assert(serialized && "Failed to serialize Protobuf payload");

        proto::ExecuteQuery server_received;
        bool parsed = server_received.ParseFromString(binary_payload);
        assert(parsed && "Server failed to parse binary payload");

        // Verify Server-side parsed message
        assert(server_received.query_id() == query_id);
        assert(server_received.statement().has_select_statement());
        assert(server_received.statement().select_statement().from_table().table() == "employees");

        std::cout << "[RESULT] PASS (Binary Payload Size: " << binary_payload.size() << " bytes)\n";
        passed_count++;
        query_id++;
    }

    std::cout << "\n=========================================================\n";
    std::cout << "  SUMMARY: " << passed_count << " / " << tests.size() << " TESTS PASSED SUCCESSFULLY! \n";
    std::cout << "=========================================================\n";

    return 0;
}
