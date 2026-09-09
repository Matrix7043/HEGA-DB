#ifndef SEMANTIC_VALIDATOR_H
#define SEMANTIC_VALIDATOR_H

#include "ast/Expr.h"
#include "ast/SelectStmt.h"
#include "catalog/Catalog.h"
#include "catalog/DataType.h"
#include "catalog/TableSchema.h"
namespace hegadb {

class SemanticValidator {

public:
  SemanticValidator(const Catalog &catalog);
  void validate(const SelectStmt &stmt) const;

private:
  const Catalog &catalog_;

  void validateProjection(const Expr *expr,
                          const TableSchema &tableSchema) const;
  void validateExpression(const Expr *expr,
                          const TableSchema &tableSchema) const;
  DataType inferType(const Expr *expr, const TableSchema &tableSchema) const;
};

} // namespace hegadb

#endif // !SEMANTIC_VALIDATOR_H
