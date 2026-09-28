#ifndef EXPRESSION_PLANNER_H
#define EXPRESSION_PLANNER_H

#include "query.pb.h"
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
    NotExpr
>;

using ExprPtr = std::shared_ptr<Expr>;

struct ColumnRefExpr {
    std::string table;
    std::string column;
};

struct ParameterRefExpr {
    uint32_t parameter_index;
};

enum class ComparisonOp {
    EQ,
    NEQ,
    LT,
    GT,
    LTE,
    GTE
};

struct ComparisonExpr {
    ExprPtr left;
    ComparisonOp op;
    ExprPtr right;
};

enum class LogicalOp {
    AND,
    OR
};

struct LogicalExpr {
    ExprPtr left;
    LogicalOp op;
    ExprPtr right;
};

struct NotExpr {
    ExprPtr child;
};

class ExpressionPlanner {
public:
    static ExprPtr plan(protocol::Expression const& proto_expr);

private:
    static ComparisonOp convertComparisonOp(protocol::ComparisonOp op);
    static LogicalOp convertLogicalOp(protocol::LogicalOp op);
};

} // namespace hegadb::server::planner

#endif // EXPRESSION_PLANNER_H

