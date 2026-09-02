#ifndef COMPARISION_EXPR_H
#define COMPARISION_EXPR_H

#include "Expr.h"

#include <memory>

namespace hegadb {

enum class ComparisonOp {
  EQ,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,
};

class ComparisonExpr : public Expr {
public:
  ComparisonExpr(std::unique_ptr<Expr> left, ComparisonOp op,
                 std::unique_ptr<Expr> right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  const Expr *left() const { return left_.get(); }
  ComparisonOp op() const { return op_; }
  const Expr *right() const { return right_.get(); }

private:
  std::unique_ptr<Expr> left_;
  ComparisonOp op_;
  std::unique_ptr<Expr> right_;
};

} // namespace hegadb

#endif
