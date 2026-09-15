#ifndef AST_NODE_H
#define AST_NODE_H

namespace hegadb::client::ast {

enum class ASTNodeKind {
    SelectStmt,
    TableRef,
    ColumnRef,
    Wildcard,
    Literal,
    ComparisonExpr,
    LogicalExpr,
    NotExpr,
    ParameterRef
};

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual ASTNodeKind kind() const = 0;
};

} // namespace hegadb

#endif
