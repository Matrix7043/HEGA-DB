#include "SQLParser.h"

#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"
#include "ast/ParameterRef.h"
#include "ast/SelectStmt.h"

#include "crypto/MockCiphertext.h"
#include "crypto/MockEncryption.h"

#include "preparation/QueryPreparer.h"

#include <cassert>
#include <iostream>
#include <variant>

int main()
{
    SQLParser parser;
    hegadb::MockEncryption encryption;
    hegadb::QueryPreparer preparer(encryption);

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        // Query should be a SELECT statement.
        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            &prepared.query());

        assert(select != nullptr);
        assert(select->where() != nullptr);

        // Literal should be replaced by ParameterRef(0).
        auto const* comparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            select->where());

        assert(comparison != nullptr);
        assert(comparison->op() == hegadb::ComparisonOp::GT);

        auto const* column = dynamic_cast<hegadb::ColumnRef const*>(
            comparison->left());

        assert(column != nullptr);
        assert(column->name() == "GRADE");

        auto const* parameter = dynamic_cast<hegadb::ParameterRef const*>(
            comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);

        // One encrypted parameter.
        assert(prepared.parameters().size() == 1);
        assert(prepared.parameters()[0].index() == 0);

        // Verify the mock ciphertext contains 90.
        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
            &ciphertext);

        assert(mockCiphertext != nullptr);

        auto const& value = mockCiphertext->value().value();

        assert(std::holds_alternative<long long>(value));
        assert(std::get<long long>(value) == 90);
    }

    // Multiple literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE GRADE > 90 AND CLASS = 10;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            &prepared.query());

        assert(select != nullptr);

        auto const* logical = dynamic_cast<hegadb::LogicalExpr const*>(
            select->where());

        assert(logical != nullptr);
        assert(logical->op() == hegadb::LogicalOp::AND);

        auto const* leftComparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            logical->left());

        auto const* rightComparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            logical->right());

        assert(leftComparison != nullptr);
        assert(rightComparison != nullptr);

        // GRADE > ParameterRef(0)
        auto const* leftColumn = dynamic_cast<hegadb::ColumnRef const*>(
            leftComparison->left());

        auto const* leftParameter = dynamic_cast<hegadb::ParameterRef const*>(
            leftComparison->right());

        assert(leftColumn != nullptr);
        assert(leftColumn->name() == "GRADE");

        assert(leftParameter != nullptr);
        assert(leftParameter->parameterIndex() == 0);

        // CLASS = ParameterRef(1)
        auto const* rightColumn = dynamic_cast<hegadb::ColumnRef const*>(
            rightComparison->left());

        auto const* rightParameter = dynamic_cast<hegadb::ParameterRef const*>(
            rightComparison->right());

        assert(rightColumn != nullptr);
        assert(rightColumn->name() == "CLASS");

        assert(rightParameter != nullptr);
        assert(rightParameter->parameterIndex() == 1);

        // Two encrypted parameters.
        assert(prepared.parameters().size() == 2);

        assert(prepared.parameters()[0].index() == 0);
        assert(prepared.parameters()[1].index() == 1);

        // Parameter 0 -> 90
        {
            auto const& ciphertext = prepared.parameters()[0].ciphertext();

            auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
                &ciphertext);

            assert(mockCiphertext != nullptr);

            auto const& value = mockCiphertext->value().value();

            assert(std::holds_alternative<long long>(value));
            assert(std::get<long long>(value) == 90);
        }

        // Parameter 1 -> 10
        {
            auto const& ciphertext = prepared.parameters()[1].ciphertext();

            auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
                &ciphertext);

            assert(mockCiphertext != nullptr);

            auto const& value = mockCiphertext->value().value();

            assert(std::holds_alternative<long long>(value));
            assert(std::get<long long>(value) == 10);
        }
    }

    // NOT expression
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE NOT (GRADE > 90);");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            &prepared.query());

        assert(select != nullptr);

        auto const* notExpr = dynamic_cast<hegadb::NotExpr const*>(
            select->where());

        assert(notExpr != nullptr);

        auto const* comparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            notExpr->child());

        assert(comparison != nullptr);

        auto const* parameter = dynamic_cast<hegadb::ParameterRef const*>(
            comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);

        // Verify encrypted parameter.
        assert(prepared.parameters().size() == 1);
        assert(prepared.parameters()[0].index() == 0);

        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
            &ciphertext);

        assert(mockCiphertext != nullptr);

        auto const& value = mockCiphertext->value().value();

        assert(std::holds_alternative<long long>(value));
        assert(std::get<long long>(value) == 90);
    }

    // No literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            &prepared.query());

        assert(select != nullptr);

        assert(select->where() == nullptr);
        assert(prepared.parameters().empty());
    }

    std::cout << "Query preparer tests passed.\n";

    return 0;
}
