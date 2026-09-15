#ifndef COMPARISION_EXPR_H
#define COMPARISION_EXPR_H

#include "Expr.h"
#include "hegadb/operations/ComparisonOp.h"

#include <memory>

namespace hegadb::client::ast {

class ComparisonExpr : public Expr {
public:
    ComparisonExpr(std::unique_ptr<Expr> left, operations::ComparisonOp op,
        std::unique_ptr<Expr> right)
        : left_(std::move(left))
        , op_(op)
        , right_(std::move(right))
    {
    }
    Expr const* left() const { return left_.get(); }
    operations::ComparisonOp op() const { return op_; }
    Expr const* right() const { return right_.get(); }
    ASTNodeKind kind() const override { return ASTNodeKind::ComparisonExpr; }

private:
    std::unique_ptr<Expr> left_;
    operations::ComparisonOp op_;
    std::unique_ptr<Expr> right_;
};

} // namespace hegadb

#endif
