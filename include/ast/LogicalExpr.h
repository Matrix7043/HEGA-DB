#ifndef LOGICAL_EXPR_H
#define LOGICAL_EXPR_H

#include "Expr.h"

#include <memory>
#include <utility>

namespace hegadb {

enum class LogicalOp { AND,
    OR };

class LogicalExpr : public Expr {

public:
    LogicalExpr(std::unique_ptr<Expr> left, LogicalOp op,
        std::unique_ptr<Expr> right)
        : left_(std::move(left))
        , op_(op)
        , right_(std::move(right))
    {
    }

    Expr const* left() const { return left_.get(); }

    LogicalOp op() const { return op_; }

    Expr const* right() const { return right_.get(); }

    ASTNodeKind kind() const override { return ASTNodeKind::LogicalExpr; }

private:
    std::unique_ptr<Expr> left_;
    LogicalOp op_;
    std::unique_ptr<Expr> right_;
};

} // namespace hegadb

#endif
