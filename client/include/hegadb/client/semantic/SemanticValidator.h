#ifndef SEMANTIC_VALIDATOR_H
#define SEMANTIC_VALIDATOR_H

#include "ast/ComparisonExpr.h"
#include "ast/Expr.h"
#include "ast/SelectStmt.h"
#include "catalog/Catalog.h"
#include "catalog/DataType.h"
#include "catalog/TableSchema.h"
namespace hegadb {

class SemanticValidator {

public:
    SemanticValidator(Catalog const& catalog);
    void validate(SelectStmt const& stmt) const;

private:
    Catalog const& catalog_;

    void validateProjection(Expr const* expr,
        TableSchema const& tableSchema) const;
    void validateExpression(Expr const* expr,
        TableSchema const& tableSchema) const;
    DataType inferType(Expr const* expr, TableSchema const& tableSchema) const;

    void validateComparison(ComparisonExpr const* expr,
        TableSchema const& table) const;

    static bool areComparable(DataType left, DataType right);
};

} // namespace hegadb

#endif // !SEMANTIC_VALIDATOR_H
