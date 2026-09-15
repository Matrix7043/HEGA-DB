#include <hegadb/client/SQLParser.h>

#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/Literal.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/NotExpr.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/ast/SelectStmt.h>

#include <hegadb/client/ast/ASTParameterizer.h>

#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>

#include <cassert>
#include <iostream>

int main()
{
    hegadb::client::SQLParser parser;
    hegadb::client::ast::ASTParameterizer parameterizer;

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::client::ast::SelectStmt const*>(
            parameterized.get());

        assert(select != nullptr);
        assert(select->where() != nullptr);

        auto const* comparison = dynamic_cast<hegadb::client::ast::ComparisonExpr const*>(
            select->where());

        assert(comparison != nullptr);

        assert(
            comparison->op() == hegadb::operations::ComparisonOp::GT);

        auto const* left = dynamic_cast<hegadb::client::ast::ColumnRef const*>(
            comparison->left());

        assert(left != nullptr);
        assert(left->name() == "GRADE");

        auto const* parameter = dynamic_cast<hegadb::client::ast::ParameterRef const*>(
            comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);
    }

    // Multiple literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE GRADE > 90 AND CLASS = 10;");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::client::ast::SelectStmt const*>(
            parameterized.get());

        assert(select != nullptr);

        auto const* logical = dynamic_cast<hegadb::client::ast::LogicalExpr const*>(
            select->where());

        assert(logical != nullptr);

        assert(
            logical->op() == hegadb::operations::LogicalOp::AND);

        auto const* leftComparison = dynamic_cast<hegadb::client::ast::ComparisonExpr const*>(
            logical->left());

        auto const* rightComparison = dynamic_cast<hegadb::client::ast::ComparisonExpr const*>(
            logical->right());

        assert(leftComparison != nullptr);
        assert(rightComparison != nullptr);

        auto const* leftParameter = dynamic_cast<hegadb::client::ast::ParameterRef const*>(
            leftComparison->right());

        auto const* rightParameter = dynamic_cast<hegadb::client::ast::ParameterRef const*>(
            rightComparison->right());

        assert(leftParameter != nullptr);
        assert(rightParameter != nullptr);

        assert(leftParameter->parameterIndex() == 0);
        assert(rightParameter->parameterIndex() == 1);

        auto const* leftColumn = dynamic_cast<hegadb::client::ast::ColumnRef const*>(
            leftComparison->left());

        auto const* rightColumn = dynamic_cast<hegadb::client::ast::ColumnRef const*>(
            rightComparison->left());

        assert(leftColumn != nullptr);
        assert(rightColumn != nullptr);

        assert(leftColumn->name() == "GRADE");
        assert(rightColumn->name() == "CLASS");
    }

    // NOT expression
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE NOT (GRADE > 90);");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::client::ast::SelectStmt const*>(
            parameterized.get());

        assert(select != nullptr);

        auto const* notExpr = dynamic_cast<hegadb::client::ast::NotExpr const*>(
            select->where());

        assert(notExpr != nullptr);

        auto const* comparison = dynamic_cast<hegadb::client::ast::ComparisonExpr const*>(
            notExpr->child());

        assert(comparison != nullptr);

        auto const* parameter = dynamic_cast<hegadb::client::ast::ParameterRef const*>(
            comparison->right());

        assert(parameter != nullptr);
        assert(parameter->parameterIndex() == 0);
    }

    // No literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS;");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::client::ast::SelectStmt const*>(
            parameterized.get());

        assert(select != nullptr);
        assert(select->where() == nullptr);
    }

    // Original AST must remain unchanged
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto const* originalWhere = ast->where();

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* originalComparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(
            originalWhere);

        assert(originalComparison != nullptr);

        assert(
            dynamic_cast<
                hegadb::client::ast::Literal const*>(
                originalComparison->right())
            != nullptr);

        auto const* newSelect = dynamic_cast<hegadb::client::ast::SelectStmt const*>(
            parameterized.get());

        auto const* newComparison = dynamic_cast<
            hegadb::client::ast::ComparisonExpr const*>(
            newSelect->where());

        assert(
            dynamic_cast<
                hegadb::client::ast::ParameterRef const*>(
                newComparison->right())
            != nullptr);
    }

    std::cout << "AST parameterizer tests passed.\n";

    return 0;
}
