#include "semantic/SemanticValidator.h"
#include "catalog/DataType.h"
#include "catalog/TableSchema.h"
#include "semantic/SemanticError.h"

#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/Literal.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"

#include <stdexcept>
#include <string>

namespace hegadb {

SemanticValidator::SemanticValidator(const Catalog &catalog)
    : catalog_(catalog) {}

void SemanticValidator::validate(const SelectStmt &stmt) const {

  const TableRef *from = stmt.from();

  if (from == nullptr) {
    throw SemanticError(SemanticErrorKind::TableNotFound,
                        "SELECT statement has no FROM table");
  }

  const TableSchema *table = catalog_.findTable(from->name());

  if (table == nullptr) {
    throw SemanticError(SemanticErrorKind::TableNotFound,
                        "table '" + from->name() + "' does not exist");
  }

  for (const auto &projection : stmt.projections()) {
    validateProjection(projection.get(), *table);
  }

  if (stmt.where() != nullptr) {
    validateExpression(stmt.where(), *table);
  }
}

void SemanticValidator::validateProjection(const Expr *expr,
                                           const TableSchema &table) const {

  if (expr == nullptr) {
    return;
  }

  if (expr->kind() == ASTNodeKind::Wildcard) {
    return;
  }

  if (expr->kind() == ASTNodeKind::ColumnRef) {

    const auto *column = static_cast<const ColumnRef *>(expr);

    if (table.findColumn(column->name()) == nullptr) {
      throw SemanticError(SemanticErrorKind::ColumnNotFound,
                          "column '" + column->name() +
                              "' does not exist in table '" + table.name() +
                              "'");
    }

    return;
  }

  throw SemanticError(SemanticErrorKind::TypeMismatch,
                      "invalid expression in SELECT projection");
}

void SemanticValidator::validateExpression(const Expr *expr,
                                           const TableSchema &table) const {

  if (expr == nullptr) {
    return;
  }

  switch (expr->kind()) {

  case ASTNodeKind::ColumnRef: {
    // inferType() also performs column resolution.
    inferType(expr, table);
    break;
  }

  case ASTNodeKind::Literal: {
    // Verify that the literal has a supported type.
    inferType(expr, table);
    break;
  }

  case ASTNodeKind::ComparisonExpr: {
    const auto *comparison = static_cast<const ComparisonExpr *>(expr);

    validateComparison(comparison, table);
    break;
  }

  case ASTNodeKind::LogicalExpr: {
    const auto *logical = static_cast<const LogicalExpr *>(expr);

    validateExpression(logical->left(), table);
    validateExpression(logical->right(), table);

    const DataType leftType = inferType(logical->left(), table);

    const DataType rightType = inferType(logical->right(), table);

    if (leftType != DataType::Boolean || rightType != DataType::Boolean) {

      throw SemanticError(SemanticErrorKind::TypeMismatch,
                          "logical operators require boolean expressions");
    }

    break;
  }

  case ASTNodeKind::NotExpr: {
    const auto *notExpr = static_cast<const NotExpr *>(expr);

    validateExpression(notExpr->child(), table);

    const DataType childType = inferType(notExpr->child(), table);

    if (childType != DataType::Boolean) {
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

DataType SemanticValidator::inferType(const Expr *expr,
                                      const TableSchema &table) const {

  if (expr == nullptr) {
    throw std::invalid_argument("Cannot infer type of null expression");
  }

  if (expr->kind() == ASTNodeKind::ColumnRef) {

    const auto *column = static_cast<const ColumnRef *>(expr);

    const ColumnSchema *schema = table.findColumn(column->name());

    if (schema == nullptr) {
      throw SemanticError(SemanticErrorKind::ColumnNotFound,
                          "column '" + column->name() +
                              "' does not exist in table '" + table.name() +
                              "'");
    }

    return schema->type;
  }

  if (expr->kind() == ASTNodeKind::Literal) {

    const auto *literal = static_cast<const Literal *>(expr);

    const auto &value = literal->value();

    if (std::holds_alternative<long long>(value)) {
      return DataType::Integer;
    }

    if (std::holds_alternative<std::string>(value)) {
      return DataType::String;
    }

    if (std::holds_alternative<bool>(value)) {
      return DataType::Boolean;
    }
  }

  if (expr->kind() == ASTNodeKind::ComparisonExpr) {
    return DataType::Boolean;
  }

  if (expr->kind() == ASTNodeKind::LogicalExpr) {
    return DataType::Boolean;
  }

  if (expr->kind() == ASTNodeKind::NotExpr) {
    return DataType::Boolean;
  }

  throw std::runtime_error(
      "Type inference not implemented for this expression");
}

bool SemanticValidator::areComparable(DataType left, DataType right) {

  if (left == right) {
    return true;
  }

  if (((left == DataType::Integer) && (right == DataType::Float)) ||
      ((left == DataType::Float) && (right == DataType::Integer))) {
    return true;
  }

  return false;
}

void SemanticValidator::validateComparison(const ComparisonExpr *expr,
                                           const TableSchema &table) const {

  const DataType leftType = inferType(expr->left(), table);
  const DataType rightType = inferType(expr->right(), table);

  if (!areComparable(leftType, rightType)) {
    throw SemanticError(SemanticErrorKind::TypeMismatch,
                        "incompatible types in comparison");
  }
}

} // namespace hegadb
