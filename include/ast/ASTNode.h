#ifndef AST_NODE_H
#define AST_NODE_H

namespace hegadb {

enum class ASTNodeKind {
  SelectStmt,
  TableRef,
  ColumnRef,
  Wildcard,
  Literal,
  ComparisonExpr,
  LogicalExpr,
  NotExpr
};

class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual ASTNodeKind kind() const = 0;
};
} // namespace hegadb

#endif
