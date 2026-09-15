#ifndef NOT_EXPR_H
#define NOT_EXPR_H

#include "Expr.h"

#include <memory>
#include <utility>

namespace hegadb::client::ast {

class NotExpr : public Expr {

public:
    explicit NotExpr(std::unique_ptr<Expr> child)
        : child_(std::move(child))
    {
    }

    Expr const* child() const { return child_.get(); }

    ASTNodeKind kind() const override { return ASTNodeKind::NotExpr; }

private:
    std::unique_ptr<Expr> child_;
};

} // namespace hegadb

#endif
