#include "hegadb/client/semantic/SemanticValidator.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/client/semantic/SemanticError.h"
#include "hegadb/types/DataType.h"

#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"

#include <stdexcept>
#include <string>

namespace hegadb::client::semantic {

SemanticValidator::SemanticValidator(catalog::Catalog const& catalog)
    : catalog_(catalog)
{
}

void SemanticValidator::validate(ast::SelectStmt const& stmt) const
{

    ast::TableRef const* from = stmt.from();

    if (from == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "SELECT statement has no FROM table");
    }

    catalog::TableSchema const* table = catalog_.findTable(from->name());

    if (table == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + from->name() + "' does not exist");
    }

    for (auto const& projection : stmt.projections()) {
        validateProjection(projection.get(), *table);
    }

    if (stmt.where() != nullptr) {

        validateExpression(stmt.where(), *table);

        types::DataType const whereType = inferType(stmt.where(), *table);

        if (whereType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "WHERE expression must evaluate to Boolean");
        }
    }
}

void SemanticValidator::validateProjection(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        return;
    }

    if (expr->kind() == ast::ASTNodeKind::Wildcard) {
        return;
    }

    if (expr->kind() == ast::ASTNodeKind::ColumnRef) {

        auto const* column = static_cast<ast::ColumnRef const*>(expr);

        if (table.findColumn(column->name()) == nullptr) {
            throw SemanticError(SemanticErrorKind::ColumnNotFound,
                "column '" + column->name() + "' does not exist in table '" + table.name() + "'");
        }

        return;
    }

    throw SemanticError(SemanticErrorKind::TypeMismatch,
        "invalid expression in SELECT projection");
}

void SemanticValidator::validateExpression(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        return;
    }

    switch (expr->kind()) {

    case ast::ASTNodeKind::ColumnRef: {
        inferType(expr, table);
        break;
    }

    case ast::ASTNodeKind::Literal: {
        inferType(expr, table);
        break;
    }

    case ast::ASTNodeKind::ComparisonExpr: {
        auto const* comparison = static_cast<ast::ComparisonExpr const*>(expr);

        validateComparison(comparison, table);
        break;
    }

    case ast::ASTNodeKind::LogicalExpr: {
        auto const* logical = static_cast<ast::LogicalExpr const*>(expr);

        validateExpression(logical->left(), table);
        validateExpression(logical->right(), table);

        types::DataType const leftType = inferType(logical->left(), table);

        types::DataType const rightType = inferType(logical->right(), table);

        if (leftType != types::DataType::Boolean || rightType != types::DataType::Boolean) {

            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "logical operators require boolean expressions");
        }

        break;
    }

    case ast::ASTNodeKind::NotExpr: {
        auto const* notExpr = static_cast<ast::NotExpr const*>(expr);

        validateExpression(notExpr->child(), table);

        types::DataType const childType = inferType(notExpr->child(), table);

        if (childType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "NOT requires a boolean expression");
        }

        break;
    }

    default:
        throw SemanticError(SemanticErrorKind::TypeMismatch,
            "invalid expression in WHERE clause");
    }
}

types::DataType SemanticValidator::inferType(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        throw std::invalid_argument("Cannot infer type of null expression");
    }

    if (expr->kind() == ast::ASTNodeKind::ColumnRef) {

        auto const* column = static_cast<ast::ColumnRef const*>(expr);

        catalog::ColumnSchema const* schema = table.findColumn(column->name());

        if (schema == nullptr) {
            throw SemanticError(SemanticErrorKind::ColumnNotFound,
                "column '" + column->name() + "' does not exist in table '" + table.name() + "'");
        }

        return schema->type;
    }

    if (expr->kind() == ast::ASTNodeKind::Literal) {

        auto const* literal = static_cast<ast::Literal const*>(expr);

        auto const& value = literal->value();

        if (std::holds_alternative<long long>(value)) {
            return types::DataType::Integer;
        }

        if (std::holds_alternative<std::string>(value)) {
            return types::DataType::String;
        }

        if (std::holds_alternative<bool>(value)) {
            return types::DataType::Boolean;
        }
    }

    if (expr->kind() == ast::ASTNodeKind::ComparisonExpr) {

        auto const* comparison = static_cast<ast::ComparisonExpr const*>(expr);
        types::DataType const leftType = inferType(comparison->left(), table);
        types::DataType const rightType = inferType(comparison->right(), table);
        if (leftType != rightType) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "comparison operands must have compatible types");
        }
        return types::DataType::Boolean;
    }

    if (expr->kind() == ast::ASTNodeKind::LogicalExpr) {

        auto const* logical = static_cast<ast::LogicalExpr const*>(expr);
        types::DataType const leftType = inferType(logical->left(), table);
        types::DataType const rightType = inferType(logical->right(), table);
        if (leftType != types::DataType::Boolean || rightType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "logical operands must have compatible types");
        }
        return types::DataType::Boolean;
    }

    if (expr->kind() == ast::ASTNodeKind::NotExpr) {
        auto const* notExpr = static_cast<ast::NotExpr const*>(expr);
        types::DataType const childType = inferType(notExpr->child(), table);
        if (childType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "NOT operand must be Boolean");
        }
        return types::DataType::Boolean;
    }

    throw std::runtime_error(
        "Type inference not implemented for this expression");
}

bool SemanticValidator::areComparable(types::DataType left, types::DataType right)
{

    if (left == right) {
        return true;
    }

    if (((left == types::DataType::Integer) && (right == types::DataType::Float)) || ((left == types::DataType::Float) && (right == types::DataType::Integer))) {
        return true;
    }

    return false;
}

void SemanticValidator::validateComparison(ast::ComparisonExpr const* expr,
    catalog::TableSchema const& table) const
{

    types::DataType const leftType = inferType(expr->left(), table);
    types::DataType const rightType = inferType(expr->right(), table);

    if (!areComparable(leftType, rightType)) {
        throw SemanticError(SemanticErrorKind::TypeMismatch,
            "incompatible types in comparison");
    }
}

} // namespace hegadb
