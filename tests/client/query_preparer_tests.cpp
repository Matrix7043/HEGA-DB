#include <hegadb/client/SQLParser.h>

#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/NotExpr.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/ast/SelectStmt.h>

#include <hegadb/client/preparation/QueryPreparer.h>

#include <hegadb/crypto/MockCiphertext.h>
#include <hegadb/crypto/MockEncryption.h>

#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>

#include <cassert>
#include <iostream>
#include <variant>

int main()
{
    hegadb::client::SQLParser parser;
    hegadb::crypto::MockEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        auto const* select = dynamic_cast<
            hegadb::client::ast::SelectStmt const*>(&prepared.query());

        assert(select != nullptr);
        assert(select->where() != nullptr);

        auto const* comparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(select->where());

        assert(comparison != nullptr);

        assert(
            comparison->op() == hegadb::operations::ComparisonOp::GT);

        auto const* column = dynamic_cast<
            hegadb::client::ast::ColumnRef const*>(comparison->left());

        assert(column != nullptr);
        assert(column->name() == "GRADE");

        auto const* parameter = dynamic_cast<
            hegadb::client::ast::ParameterRef const*>(comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);

        assert(prepared.parameters().size() == 1);
        assert(prepared.parameters()[0].index() == 0);

        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<
            hegadb::crypto::MockCiphertext const*>(&ciphertext);

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

        auto const* select = dynamic_cast<
            hegadb::client::ast::SelectStmt const*>(&prepared.query());

        assert(select != nullptr);

        auto const* logical = dynamic_cast<
            hegadb::client::ast::LogicalExpr const*>(select->where());

        assert(logical != nullptr);

        assert(
            logical->op() == hegadb::operations::LogicalOp::AND);

        auto const* leftComparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(logical->left());

        auto const* rightComparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(logical->right());

        assert(leftComparison != nullptr);
        assert(rightComparison != nullptr);

        auto const* leftColumn = dynamic_cast<
            hegadb::client::ast::ColumnRef const*>(leftComparison->left());

        auto const* leftParameter = dynamic_cast<
            hegadb::client::ast::ParameterRef const*>(leftComparison->right());

        assert(leftColumn != nullptr);
        assert(leftColumn->name() == "GRADE");

        assert(leftParameter != nullptr);
        assert(leftParameter->parameterIndex() == 0);

        auto const* rightColumn = dynamic_cast<
            hegadb::client::ast::ColumnRef const*>(rightComparison->left());

        auto const* rightParameter = dynamic_cast<
            hegadb::client::ast::ParameterRef const*>(rightComparison->right());

        assert(rightColumn != nullptr);
        assert(rightColumn->name() == "CLASS");

        assert(rightParameter != nullptr);
        assert(rightParameter->parameterIndex() == 1);

        assert(prepared.parameters().size() == 2);

        assert(prepared.parameters()[0].index() == 0);
        assert(prepared.parameters()[1].index() == 1);

        {
            auto const& ciphertext = prepared.parameters()[0].ciphertext();

            auto const* mockCiphertext = dynamic_cast<
                hegadb::crypto::MockCiphertext const*>(&ciphertext);

            assert(mockCiphertext != nullptr);

            auto const& value = mockCiphertext->value().value();

            assert(std::holds_alternative<long long>(value));
            assert(std::get<long long>(value) == 90);
        }

        {
            auto const& ciphertext = prepared.parameters()[1].ciphertext();

            auto const* mockCiphertext = dynamic_cast<
                hegadb::crypto::MockCiphertext const*>(&ciphertext);

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

        auto const* select = dynamic_cast<
            hegadb::client::ast::SelectStmt const*>(&prepared.query());

        assert(select != nullptr);

        auto const* notExpr = dynamic_cast<
            hegadb::client::ast::NotExpr const*>(select->where());

        assert(notExpr != nullptr);

        auto const* comparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(notExpr->child());

        assert(comparison != nullptr);

        auto const* parameter = dynamic_cast<
            hegadb::client::ast::ParameterRef const*>(comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);

        assert(prepared.parameters().size() == 1);

        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<
            hegadb::crypto::MockCiphertext const*>(&ciphertext);

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

        auto const* select = dynamic_cast<
            hegadb::client::ast::SelectStmt const*>(&prepared.query());

        assert(select != nullptr);

        assert(select->where() == nullptr);
        assert(prepared.parameters().empty());
    }

    std::cout
        << "Query preparer tests passed.\n";

    return 0;
}
