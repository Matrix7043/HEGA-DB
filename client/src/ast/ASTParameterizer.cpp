#include "hegadb/client/ast/ASTParameterizer.h"
#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/ParameterRef.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/ast/TableRef.h"
#include "hegadb/client/ast/Wildcard.h"

#include <stdexcept>
#include <utility>

namespace hegadb::client::ast {

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

        return std::make_unique<ColumnRef>(
            column->name());
    }

    if (dynamic_cast<Wildcard const*>(&node)) {
        return std::make_unique<Wildcard>();
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
    auto const* select = dynamic_cast<SelectStmt const*>(&root);

    if (!select) {
        throw std::runtime_error(
            "ASTParameterizer expects a SelectStmt");
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

    if (select->where()) {
        result->setWhere(
            transform(
                *select->where(),
                next_parameter_index));
    }

    return result;
}

}
