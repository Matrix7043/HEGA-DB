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
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace ast = hegadb::client::ast;
namespace proto = hegadb::protocol;
namespace ops = hegadb::operations;

namespace {

constexpr uint32_t PROTOCOL_MAJOR_VERSION = 0;
constexpr uint32_t PROTOCOL_MINOR_VERSION = 6;

struct TestCase {
    std::string name;
    std::string sql;
    bool expect_success;
};

/*
 * ---------------------------------------------------------
 * Operation -> Protocol enum mappings
 * ---------------------------------------------------------
 */

proto::ComparisonOp mapComparisonOp(ops::ComparisonOp op)
{
    switch (op) {
    case ops::ComparisonOp::EQ:
        return proto::COMPARISON_OP_EQ;

    case ops::ComparisonOp::NEQ:
        return proto::COMPARISON_OP_NEQ;

    case ops::ComparisonOp::LT:
        return proto::COMPARISON_OP_LT;

    case ops::ComparisonOp::GT:
        return proto::COMPARISON_OP_GT;

    case ops::ComparisonOp::LTE:
        return proto::COMPARISON_OP_LTE;

    case ops::ComparisonOp::GTE:
        return proto::COMPARISON_OP_GTE;
    }

    assert(false && "Unknown ComparisonOp");
    return proto::COMPARISON_OP_UNSPECIFIED;
}

proto::LogicalOp mapLogicalOp(ops::LogicalOp op)
{
    switch (op) {
    case ops::LogicalOp::AND:
        return proto::LOGICAL_OP_AND;

    case ops::LogicalOp::OR:
        return proto::LOGICAL_OP_OR;
    }

    assert(false && "Unknown LogicalOp");
    return proto::LOGICAL_OP_UNSPECIFIED;
}

/*
 * ---------------------------------------------------------
 * AST Expression -> Protocol Expression
 * ---------------------------------------------------------
 */

void convertExpression(
    ast::Expr const* node,
    proto::Expression* proto_expr)
{
    assert(proto_expr != nullptr);

    if (!node) {
        return;
    }

    if (auto const* column = dynamic_cast<ast::ColumnRef const*>(node)) {

        proto_expr->mutable_column_ref()->set_column(
            column->name());

    } else if (auto const* parameter = dynamic_cast<ast::ParameterRef const*>(node)) {

        proto_expr->mutable_parameter_ref()->set_parameter_index(
            parameter->parameterIndex());

    } else if (dynamic_cast<ast::Wildcard const*>(node)) {

        proto_expr->mutable_wildcard();

    } else if (auto const* comparison = dynamic_cast<ast::ComparisonExpr const*>(node)) {

        auto* proto_comparison = proto_expr->mutable_comparison_expr();

        convertExpression(
            comparison->left(),
            proto_comparison->mutable_left());

        proto_comparison->set_operator_(
            mapComparisonOp(comparison->op()));

        convertExpression(
            comparison->right(),
            proto_comparison->mutable_right());

    } else if (auto const* logical = dynamic_cast<ast::LogicalExpr const*>(node)) {

        auto* proto_logical = proto_expr->mutable_logical_expr();

        convertExpression(
            logical->left(),
            proto_logical->mutable_left());

        proto_logical->set_operator_(
            mapLogicalOp(logical->op()));

        convertExpression(
            logical->right(),
            proto_logical->mutable_right());

    } else if (auto const* not_expr = dynamic_cast<ast::NotExpr const*>(node)) {

        auto* proto_not = proto_expr->mutable_not_expr();

        convertExpression(
            not_expr->child(),
            proto_not->mutable_child());
    }
}

/*
 * ---------------------------------------------------------
 * PreparedQuery -> ExecuteQuery
 * ---------------------------------------------------------
 */

proto::ExecuteQuery toProto(
    hegadb::client::preparation::PreparedQuery const& prepared,
    std::string const& request_id)
{
    proto::ExecuteQuery request;

    /*
     * Protocol version
     */
    request.mutable_protocol_version()->set_major(
        PROTOCOL_MAJOR_VERSION);

    request.mutable_protocol_version()->set_minor(
        PROTOCOL_MINOR_VERSION);

    /*
     * Request ID
     */
    request.mutable_request_id()->set_value(
        request_id);

    /*
     * Prepared query must currently be a SELECT.
     */
    auto const* select_ast = dynamic_cast<ast::SelectStmt const*>(
        &prepared.query());

    assert(
        select_ast != nullptr && "Prepared query must be a SelectStmt");

    auto* select = request.mutable_statement()
                       ->mutable_select_statement();

    /*
     * FROM table
     */
    if (select_ast->from()) {
        select->mutable_from_table()->set_table(
            select_ast->from()->name());
    }

    /*
     * Projection list
     */
    for (auto const& projection :
        select_ast->projections()) {

        convertExpression(
            projection.get(),
            select->add_projections());
    }

    /*
     * WHERE expression
     */
    if (select_ast->where()) {

        convertExpression(
            select_ast->where(),
            select->mutable_where());
    }

    /*
     * Encrypted parameters.
     *
     * Mock ciphertext is intentional here.
     * This test verifies protocol handling rather
     * than actual FHE encryption.
     */
    for (auto const& parameter :
        prepared.parameters()) {

        auto* proto_parameter = request.add_parameters();

        proto_parameter->set_index(
            parameter.index());

        proto_parameter->set_ciphertext(
            "MOCK_ENCRYPTED_BYTES");
    }

    return request;
}

/*
 * ---------------------------------------------------------
 * Protocol Version Test
 * ---------------------------------------------------------
 */

void testProtocolVersion()
{
    proto::ProtocolVersion version;

    version.set_major(PROTOCOL_MAJOR_VERSION);
    version.set_minor(PROTOCOL_MINOR_VERSION);

    assert(
        version.major() == PROTOCOL_MAJOR_VERSION);

    assert(
        version.minor() == PROTOCOL_MINOR_VERSION);

    std::cout
        << "[PASS] ProtocolVersion "
        << version.major()
        << "."
        << version.minor()
        << "\n";
}

/*
 * ---------------------------------------------------------
 * Request ID Test
 * ---------------------------------------------------------
 */

void testRequestId()
{
    proto::RequestId request_id;

    request_id.set_value(
        "protocol-test-001");

    assert(
        request_id.value() == "protocol-test-001");

    std::cout
        << "[PASS] RequestId\n";
}

/*
 * ---------------------------------------------------------
 * Enum Test
 * ---------------------------------------------------------
 */

void testProtocolEnums()
{
    /*
     * Comparison operations.
     */
    assert(
        proto::COMPARISON_OP_UNSPECIFIED == 0);

    assert(
        proto::COMPARISON_OP_EQ != proto::COMPARISON_OP_UNSPECIFIED);

    assert(
        proto::COMPARISON_OP_NEQ != proto::COMPARISON_OP_UNSPECIFIED);

    assert(
        proto::COMPARISON_OP_LT != proto::COMPARISON_OP_UNSPECIFIED);

    assert(
        proto::COMPARISON_OP_GT != proto::COMPARISON_OP_UNSPECIFIED);

    assert(
        proto::COMPARISON_OP_LTE != proto::COMPARISON_OP_UNSPECIFIED);

    assert(
        proto::COMPARISON_OP_GTE != proto::COMPARISON_OP_UNSPECIFIED);

    /*
     * Logical operations.
     */
    assert(
        proto::LOGICAL_OP_UNSPECIFIED == 0);

    assert(
        proto::LOGICAL_OP_AND != proto::LOGICAL_OP_UNSPECIFIED);

    assert(
        proto::LOGICAL_OP_OR != proto::LOGICAL_OP_UNSPECIFIED);

    /*
     * Status.
     */
    assert(
        proto::STATUS_OK == 0);

    assert(
        proto::STATUS_ERROR != proto::STATUS_OK);

    assert(
        proto::STATUS_SYNTAX_ERROR != proto::STATUS_OK);

    /*
     * Message types.
     */
    assert(
        proto::MESSAGE_TYPE_UNSPECIFIED == 0);

    assert(
        proto::QUERY_REQUEST != proto::MESSAGE_TYPE_UNSPECIFIED);

    assert(
        proto::QUERY_RESPONSE != proto::MESSAGE_TYPE_UNSPECIFIED);

    assert(
        proto::ERROR_RESPONSE != proto::MESSAGE_TYPE_UNSPECIFIED);

    std::cout
        << "[PASS] Protocol enums\n";
}

/*
 * ---------------------------------------------------------
 * QueryResponse Test
 * ---------------------------------------------------------
 */

void testQueryResponse()
{
    proto::QueryResponse response;

    response.mutable_protocol_version()->set_major(
        PROTOCOL_MAJOR_VERSION);

    response.mutable_protocol_version()->set_minor(
        PROTOCOL_MINOR_VERSION);

    response.mutable_request_id()->set_value(
        "protocol-test-001");

    response.mutable_status()->set_code(
        proto::STATUS_OK);

    response.mutable_status()->set_message(
        "Query executed successfully");

    assert(
        response.protocol_version().major() == PROTOCOL_MAJOR_VERSION);

    assert(
        response.protocol_version().minor() == PROTOCOL_MINOR_VERSION);

    assert(
        response.request_id().value() == "protocol-test-001");

    assert(
        response.status().code() == proto::STATUS_OK);

    assert(
        response.status().message() == "Query executed successfully");

    std::cout
        << "[PASS] QueryResponse\n";
}

/*
 * ---------------------------------------------------------
 * Main
 * ---------------------------------------------------------
 */

} // namespace

int main()
{
    std::cout
        << "=========================================================\n";

    std::cout
        << "           HEGADB PROTOCOL TEST SUITE                  \n";

    std::cout
        << "=========================================================\n\n";

    size_t passed_count = 0;

    /*
     * -----------------------------------------------------
     * Common protocol tests
     * -----------------------------------------------------
     */

    testProtocolVersion();
    passed_count++;

    testRequestId();
    passed_count++;

    testProtocolEnums();
    passed_count++;

    testQueryResponse();
    passed_count++;

    /*
     * -----------------------------------------------------
     * Client-side protocol preparation
     * -----------------------------------------------------
     */

    hegadb::client::SQLParser parser;

    hegadb::crypto::MockEncryption encryption;

    hegadb::client::preparation::QueryPreparer preparer(
        encryption);

    std::vector<TestCase> const tests = {

        { "Basic SELECT *",
            "SELECT * FROM employees;",
            true },

        { "Single column",
            "SELECT name FROM employees;",
            true },

        { "Multiple columns",
            "SELECT name, salary, department_id "
            "FROM employees;",
            true },

        { "WHERE comparison",
            "SELECT name FROM employees "
            "WHERE salary > 50000;",
            true },

        { "WHERE string",
            "SELECT name FROM employees "
            "WHERE name = 'Alice';",
            true },

        { "WHERE AND",
            "SELECT name FROM employees "
            "WHERE salary > 50000 "
            "AND department_id = 10;",
            true },

        { "WHERE OR",
            "SELECT name FROM employees "
            "WHERE salary > 50000 "
            "OR department_id = 10;",
            true },

        { "WHERE NOT",
            "SELECT name FROM employees "
            "WHERE NOT salary > 50000;",
            true },

        { "WHERE parentheses",
            "SELECT name FROM employees "
            "WHERE (salary > 50000 OR department_id = 10) "
            "AND active = TRUE;",
            true },

        { "Missing SELECT list",
            "SELECT FROM employees;",
            false },

        { "Missing FROM",
            "SELECT name;",
            false },

        { "Missing table",
            "SELECT name FROM;",
            false },

        { "Missing WHERE expression",
            "SELECT name FROM employees WHERE;",
            false },

        { "Unsupported ORDER BY",
            "SELECT name FROM employees ORDER BY name;",
            false },

        { "Unsupported arithmetic",
            "SELECT salary + 100 FROM employees;",
            false },

        { "Unsupported JOIN",
            "SELECT * FROM employees JOIN departments;",
            false }
    };

    /*
     * -----------------------------------------------------
     * Query protocol tests
     * -----------------------------------------------------
     */

    uint32_t query_number = 1;

    for (auto const& test : tests) {

        std::cout
            << "\n---------------------------------------------------------\n";

        std::cout
            << "[Test #" << query_number << "] "
            << test.name << "\n";

        std::cout
            << "SQL: \"" << test.sql << "\"\n";

        /*
         * SQL -> AST
         */
        auto ast = parser.parseToAST(test.sql);

        /*
         * Expected failure.
         */
        if (!test.expect_success) {

            assert(
                ast == nullptr && "Expected SQL parsing to fail");

            std::cout
                << "[RESULT] PASS "
                << "(Rejected as expected)\n";

            passed_count++;
            query_number++;

            continue;
        }

        /*
         * Expected success.
         */
        assert(
            ast != nullptr && "Expected SQL parsing to succeed");

        /*
         * AST -> PreparedQuery
         */
        auto prepared = preparer.prepare(std::move(ast));

        /*
         * PreparedQuery -> Protocol ExecuteQuery
         */
        std::string request_id = "protocol-test-" + std::to_string(query_number);

        proto::ExecuteQuery request = toProto(
            prepared,
            request_id);

        /*
         * -------------------------------------------------
         * Validate protocol version
         * -------------------------------------------------
         */

        assert(
            request.protocol_version().major() == PROTOCOL_MAJOR_VERSION);

        assert(
            request.protocol_version().minor() == PROTOCOL_MINOR_VERSION);

        /*
         * -------------------------------------------------
         * Validate RequestId
         * -------------------------------------------------
         */

        assert(
            request.request_id().value() == request_id);

        /*
         * -------------------------------------------------
         * Validate Statement
         * -------------------------------------------------
         */

        assert(
            request.statement()
                .has_select_statement());

        auto const& select = request.statement()
                                 .select_statement();

        /*
         * FROM
         */
        assert(
            select.from_table().table() == "employees");

        /*
         * Projections
         */
        assert(
            select.projections_size() > 0);

        /*
         * -------------------------------------------------
         * Print protocol representation
         * -------------------------------------------------
         */

        std::string text_output;

        bool toString = google::protobuf::TextFormat::PrintToString(
            request,
            &text_output);

        assert(toString);

        std::cout
            << "\n--- PROTOBUF REPRESENTATION ---\n"
            << text_output;

        /*
         * -------------------------------------------------
         * Serialize
         * -------------------------------------------------
         */

        std::string binary_payload;

        bool serialized = request.SerializeToString(
            &binary_payload);

        assert(
            serialized && "Failed to serialize ExecuteQuery");

        assert(
            !binary_payload.empty());

        /*
         * -------------------------------------------------
         * Deserialize
         * -------------------------------------------------
         */

        proto::ExecuteQuery received;

        bool parsed = received.ParseFromString(
            binary_payload);

        assert(
            parsed && "Failed to deserialize ExecuteQuery");

        /*
         * -------------------------------------------------
         * Validate deserialized message
         * -------------------------------------------------
         */

        assert(
            received.protocol_version().major() == PROTOCOL_MAJOR_VERSION);

        assert(
            received.protocol_version().minor() == PROTOCOL_MINOR_VERSION);

        assert(
            received.request_id().value() == request_id);

        assert(
            received.statement()
                .has_select_statement());

        assert(
            received.statement()
                .select_statement()
                .from_table()
                .table()
            == "employees");

        assert(
            received.statement()
                .select_statement()
                .projections_size()
            == select.projections_size());

        /*
         * Parameters must survive serialization.
         */
        assert(
            received.parameters_size() == request.parameters_size());

        for (int i = 0;
            i < request.parameters_size();
            ++i) {

            assert(
                received.parameters(i).index() == request.parameters(i).index());

            assert(
                received.parameters(i).ciphertext() == request.parameters(i).ciphertext());
        }

        /*
         * -------------------------------------------------
         * Result
         * -------------------------------------------------
         */

        std::cout
            << "[RESULT] PASS "
            << "(Binary Payload Size: "
            << binary_payload.size()
            << " bytes)\n";

        passed_count++;
        query_number++;
    }

    /*
     * -----------------------------------------------------
     * Summary
     * -----------------------------------------------------
     */

    size_t total_tests = passed_count;

    std::cout
        << "\n=========================================================\n";

    std::cout
        << "  SUMMARY: "
        << passed_count
        << " / "
        << total_tests
        << " TESTS PASSED SUCCESSFULLY!\n";

    std::cout
        << "=========================================================\n";

    return 0;
}
