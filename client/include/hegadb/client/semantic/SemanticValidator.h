#ifndef SEMANTIC_VALIDATOR_H
#define SEMANTIC_VALIDATOR_H

#include "hegadb/catalog/Catalog.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/CreateTableStmt.h"
#include "hegadb/client/ast/DropTableStmt.h"
#include "hegadb/client/ast/Expr.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/ast/Statement.h"
#include "hegadb/types/DataType.h"

namespace hegadb::client::semantic {

class SemanticValidator {

public:
    SemanticValidator(catalog::Catalog const& catalog);

    // Dispatches on stmt.kind() to one of the overloads below.
    // Throws std::invalid_argument for an unsupported statement kind.
    void validate(ast::Statement const& stmt) const;

    void validate(ast::SelectStmt const& stmt) const;

    // The table must not already exist, must have at least one column,
    // column names must be unique, and every type must be supported.
    void validate(ast::CreateTableStmt const& stmt) const;

    // The table must exist.
    void validate(ast::DropTableStmt const& stmt) const;

private:
    catalog::Catalog const& catalog_;

    // INNER JOIN: both tables must exist and differ, the ON columns must
    // come one from each table and have the same type, every projection
    // must resolve to exactly one column, and WHERE is not allowed.
    void validateJoin(ast::SelectStmt const& stmt,
        catalog::TableSchema const& left_table) const;

    void validateProjection(ast::Expr const* expr,
        catalog::TableSchema const& tableSchema) const;
    void validateExpression(ast::Expr const* expr,
        catalog::TableSchema const& tableSchema) const;
    types::DataType inferType(ast::Expr const* expr, catalog::TableSchema const& tableSchema) const;

    void validateComparison(ast::ComparisonExpr const* expr,
        catalog::TableSchema const& table) const;

    static bool areComparable(types::DataType left, types::DataType right);
};

} // namespace hegadb

#endif // !SEMANTIC_VALIDATOR_H
