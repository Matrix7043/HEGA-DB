#ifndef EXPRESSION_PLANNER_H
#define EXPRESSION_PLANNER_H

#include "query.pb.h"
#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>
#include <memory>
#include <string>
#include <variant>

namespace hegadb::server::planner {

// Forward declarations
struct ColumnRefExpr;
struct ParameterRefExpr;
struct ComparisonExpr;
struct LogicalExpr;
struct NotExpr;

using Expr = std::variant<
    ColumnRefExpr,
    ParameterRefExpr,
    ComparisonExpr,
    LogicalExpr,
    NotExpr>;

using ExprPtr = std::shared_ptr<Expr>;

struct ColumnRefExpr {
    std::string table;
    std::string column;
};

struct ParameterRefExpr {
    uint32_t parameter_index;
};

struct ComparisonExpr {
    ExprPtr left;
    operations::ComparisonOp op;
    ExprPtr right;
};

struct LogicalExpr {
    ExprPtr left;
    operations::LogicalOp op;
    ExprPtr right;
};

struct NotExpr {
    ExprPtr child;
};

class ExpressionPlanner {
public:
    static ExprPtr plan(protocol::Expression const& proto_expr);

private:
    static operations::ComparisonOp convertComparisonOp(protocol::ComparisonOp op);
    static operations::LogicalOp convertLogicalOp(protocol::LogicalOp op);
};

} // namespace hegadb::server::planner

#endif // EXPRESSION_PLANNER_H
