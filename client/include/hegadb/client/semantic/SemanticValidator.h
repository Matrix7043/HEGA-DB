#ifndef SEMANTIC_VALIDATOR_H
#define SEMANTIC_VALIDATOR_H

#include "hegadb/catalog/Catalog.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Expr.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/types/DataType.h"

namespace hegadb::client::semantic {

class SemanticValidator {

public:
    SemanticValidator(catalog::Catalog const& catalog);
    void validate(ast::SelectStmt const& stmt) const;

private:
    catalog::Catalog const& catalog_;

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
