#include "hegadb/client/ast/ASTParameterizer.h"
#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/InsertStmt.h"
#include "hegadb/client/ast/JoinClause.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/NullLiteral.h"
#include "hegadb/client/ast/ParameterRef.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/ast/TableRef.h"
#include "hegadb/client/ast/Wildcard.h"

#include <stdexcept>
#include <utility>

namespace hegadb::client::ast {

namespace {

std::unique_ptr<ColumnRef> cloneColumn(ColumnRef const& column)
{
    if (column.table()) {
        return std::make_unique<ColumnRef>(*column.table(), column.name());
    }

    return std::make_unique<ColumnRef>(column.name());
}

} // namespace

std::unique_ptr<Expr>
ASTParameterizer::transform(
    Expr const& node,
    std::size_t& next_parameter_index) const
{
    if (dynamic_cast<Literal const*>(&node)) {
        return std::make_unique<ParameterRef>(
            next_parameter_index++);
    }

    if (auto const* column = dynamic_cast<ColumnRef const*>(&node)) {

        return cloneColumn(*column);
    }

    if (dynamic_cast<Wildcard const*>(&node)) {
        return std::make_unique<Wildcard>();
    }

    // NULL is not encrypted, so it stays as it is.
    if (dynamic_cast<NullLiteral const*>(&node)) {
        return std::make_unique<NullLiteral>();
    }

    if (auto const* comparison = dynamic_cast<ComparisonExpr const*>(&node)) {

        auto left = transform(
            *comparison->left(),
            next_parameter_index);

        auto right = transform(
            *comparison->right(),
            next_parameter_index);

        return std::make_unique<ComparisonExpr>(
            std::move(left),
            comparison->op(),
            std::move(right));
    }

    if (auto const* logical = dynamic_cast<LogicalExpr const*>(&node)) {

        auto left = transform(
            *logical->left(),
            next_parameter_index);

        auto right = transform(
            *logical->right(),
            next_parameter_index);

        return std::make_unique<LogicalExpr>(
            std::move(left),
            logical->op(),
            std::move(right));
    }

    if (auto const* notExpr = dynamic_cast<NotExpr const*>(&node)) {

        auto child = transform(
            *notExpr->child(),
            next_parameter_index);

        return std::make_unique<NotExpr>(
            std::move(child));
    }

    throw std::runtime_error(
        "Unsupported expression in AST parameterizer");
}

std::unique_ptr<ASTNode>
ASTParameterizer::parameterize(
    ASTNode const& root) const
{
    if (auto const* insert = dynamic_cast<InsertStmt const*>(&root)) {
        auto result = std::make_unique<InsertStmt>(insert->tableName());

        for (auto const& column : insert->columns()) {
            result->addColumn(column);
        }

        // Parameters are numbered in the same order LiteralExtractor
        // extracts them: row by row, left to right.
        std::size_t next_parameter_index = 0;

        for (auto const& row : insert->rows()) {
            InsertStmt::Row parameterized;

            for (auto const& cell : row) {
                parameterized.push_back(
                    transform(*cell, next_parameter_index));
            }

            result->addRow(std::move(parameterized));
        }

        return result;
    }

    auto const* select = dynamic_cast<SelectStmt const*>(&root);

    if (!select) {
        throw std::runtime_error(
            "ASTParameterizer expects a SelectStmt or an InsertStmt");
    }

    auto result = std::make_unique<SelectStmt>();

    std::size_t next_parameter_index = 0;

    for (auto const& projection :
        select->projections()) {

        result->addProjection(
            transform(
                *projection,
                next_parameter_index));
    }

    if (select->from()) {
        result->setFrom(
            std::make_unique<TableRef>(
                select->from()->name()));
    }

    // The JOIN clause has no literals, so it is copied as it is.
    if (select->join()) {
        auto const* join = select->join();

        result->setJoin(
            std::make_unique<JoinClause>(
                std::make_unique<TableRef>(join->table()->name()),
                cloneColumn(*join->left()),
                cloneColumn(*join->right())));
    }

    if (select->where()) {
        result->setWhere(
            transform(
                *select->where(),
                next_parameter_index));
    }

    return result;
}

}
