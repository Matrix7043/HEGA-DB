#include <hegadb/client/SQLParser.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>

#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/preparation/QueryPreparer.h>

#include <hegadb/crypto/MockEncryption.h>

#include <cassert>
#include <iostream>
#include <string>

namespace proto = hegadb::protocol;

int main()
{
    std::cout
        << "=========================================================\n"
        << "       HEGADB EXECUTE QUERY ENCODER TESTS              \n"
        << "=========================================================\n\n";

    /*
     * -----------------------------------------------------
     * Client-side preparation pipeline
     * -----------------------------------------------------
     */

    hegadb::client::SQLParser parser;

    hegadb::crypto::MockEncryption encryption;

    hegadb::client::preparation::QueryPreparer preparer(
        encryption);

    /*
     * -----------------------------------------------------
     * Test 1: Basic SELECT
     * -----------------------------------------------------
     */

    {
        std::cout << "[Test 1] Basic SELECT encoding\n";

        auto ast = parser.parseToAST(
            "SELECT NAME, CLASS FROM STUDENTS;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
            prepared,
            "encoder-test-001");

        /*
         * Protocol version
         */
        assert(
            request.protocol_version().major() == 1);

        assert(
            request.protocol_version().minor() == 1);

        /*
         * Request ID
         */
        assert(
            request.request_id().value()
            == "encoder-test-001");

        /*
         * Statement
         */
        assert(
            request.statement().has_select_statement());

        auto const& select = request.statement().select_statement();

        /*
         * FROM
         */
        assert(
            select.from_table().table()
            == "STUDENTS");

        /*
         * Projections
         */
        assert(
            select.projections_size() == 2);

        assert(
            select.projections(0)
                .column_ref()
                .column()
            == "NAME");

        assert(
            select.projections(1)
                .column_ref()
                .column()
            == "CLASS");

        /*
         * No parameters
         */
        assert(
            request.parameters_size() == 0);

        std::cout << "[PASS]\n\n";
    }

    /*
     * -----------------------------------------------------
     * Test 2: WHERE parameter
     * -----------------------------------------------------
     */

    {
        std::cout << "[Test 2] Parameterized WHERE encoding\n";

        auto ast = parser.parseToAST(
            "SELECT NAME FROM STUDENTS "
            "WHERE CLASS = 10;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        /*
         * QueryPreparer should have produced one
         * encrypted parameter.
         */
        assert(
            prepared.parameters().size() == 1);

        auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
            prepared,
            "encoder-test-002");

        /*
         * Request ID
         */
        assert(
            request.request_id().value()
            == "encoder-test-002");

        /*
         * Statement
         */
        assert(
            request.statement().has_select_statement());

        auto const& select = request.statement().select_statement();

        /*
         * FROM
         */
        assert(
            select.from_table().table()
            == "STUDENTS");

        /*
         * WHERE
         */
        assert(
            select.has_where());

        auto const& where = select.where();

        assert(
            where.has_comparison_expr());

        auto const& comparison = where.comparison_expr();

        /*
         * Comparison operator
         */
        assert(
            comparison.operator_()
            == proto::COMPARISON_OP_EQ);

        /*
         * Left side
         */
        assert(
            comparison.left().has_column_ref());

        assert(
            comparison.left()
                .column_ref()
                .column()
            == "CLASS");

        /*
         * Right side
         *
         * QueryPreparer should have replaced the
         * literal with a ParameterRef.
         */
        assert(
            comparison.right().has_parameter_ref());

        assert(
            comparison.right()
                .parameter_ref()
                .parameter_index()
            == 0);

        /*
         * Encrypted parameter
         */
        assert(
            request.parameters_size() == 1);

        assert(
            request.parameters(0).index() == 0);

        assert(
            request.parameters(0).ciphertext()
            == "MOCK_ENCRYPTED_BYTES");

        std::cout << "[PASS]\n\n";
    }

    /*
     * -----------------------------------------------------
     * Test 3: Logical expression
     * -----------------------------------------------------
     */

    {
        std::cout << "[Test 3] Logical expression encoding\n";

        auto ast = parser.parseToAST(
            "SELECT NAME FROM STUDENTS "
            "WHERE CLASS = 10 AND GRADE > 5;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        assert(
            prepared.parameters().size() == 2);

        auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
            prepared,
            "encoder-test-003");

        auto const& select = request.statement().select_statement();

        assert(select.has_where());

        auto const& where = select.where();

        assert(
            where.has_logical_expr());

        auto const& logical = where.logical_expr();

        assert(
            logical.operator_()
            == proto::LOGICAL_OP_AND);

        /*
         * Left comparison
         */
        assert(
            logical.left().has_comparison_expr());

        assert(
            logical.left()
                .comparison_expr()
                .operator_()
            == proto::COMPARISON_OP_EQ);

        /*
         * Right comparison
         */
        assert(
            logical.right().has_comparison_expr());

        assert(
            logical.right()
                .comparison_expr()
                .operator_()
            == proto::COMPARISON_OP_GT);

        /*
         * Both encrypted parameters
         */
        assert(
            request.parameters_size() == 2);

        assert(
            request.parameters(0).index() == 0);

        assert(
            request.parameters(1).index() == 1);

        assert(
            request.parameters(0).ciphertext()
            == "MOCK_ENCRYPTED_BYTES");

        assert(
            request.parameters(1).ciphertext()
            == "MOCK_ENCRYPTED_BYTES");

        std::cout << "[PASS]\n\n";
    }

    /*
     * -----------------------------------------------------
     * Summary
     * -----------------------------------------------------
     */

    std::cout
        << "=========================================================\n"
        << "  ALL EXECUTE QUERY ENCODER TESTS PASSED\n"
        << "=========================================================\n";

    return 0;
}
