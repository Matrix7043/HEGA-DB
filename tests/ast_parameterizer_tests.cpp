#include "SQLParser.h"

#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/Literal.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"
#include "ast/ParameterRef.h"
#include "ast/SelectStmt.h"

#include "preparation/ASTParameterizer.h"

#include <cassert>
#include <iostream>

int main()
{
    SQLParser parser;
    hegadb::ASTParameterizer parameterizer;

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            parameterized.get());

        assert(select != nullptr);
        assert(select->where() != nullptr);

        auto const* comparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            select->where());

        assert(comparison != nullptr);
        assert(comparison->op() == hegadb::ComparisonOp::GT);

        auto const* left = dynamic_cast<hegadb::ColumnRef const*>(
            comparison->left());

        assert(left != nullptr);
        assert(left->name() == "GRADE");

        auto const* parameter = dynamic_cast<hegadb::ParameterRef const*>(
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

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            parameterized.get());

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

        auto const* leftParameter = dynamic_cast<hegadb::ParameterRef const*>(
            leftComparison->right());

        auto const* rightParameter = dynamic_cast<hegadb::ParameterRef const*>(
            rightComparison->right());

        assert(leftParameter != nullptr);
        assert(rightParameter != nullptr);

        assert(leftParameter->parameterIndex() == 0);
        assert(rightParameter->parameterIndex() == 1);

        auto const* leftColumn = dynamic_cast<hegadb::ColumnRef const*>(
            leftComparison->left());

        auto const* rightColumn = dynamic_cast<hegadb::ColumnRef const*>(
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

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
            parameterized.get());

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
    }

    // No literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS;");

        assert(ast != nullptr);

        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<hegadb::SelectStmt const*>(
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

        // Original AST still contains the literal.
        auto const* originalComparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            originalWhere);

        assert(originalComparison != nullptr);
        assert(dynamic_cast<hegadb::Literal const*>(
                   originalComparison->right())
            != nullptr);

        // New AST contains ParameterRef.
        auto const* newSelect = dynamic_cast<hegadb::SelectStmt const*>(
            parameterized.get());

        auto const* newComparison = dynamic_cast<hegadb::ComparisonExpr const*>(
            newSelect->where());

        assert(dynamic_cast<hegadb::ParameterRef const*>(
                   newComparison->right())
            != nullptr);
    }

    std::cout << "AST parameterizer tests passed.\n";

    return 0;
}
