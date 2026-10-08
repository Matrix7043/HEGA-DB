#include <hegadb/server/planner/ExpressionPlanner.h>
#include <stdexcept>

namespace hegadb::server::planner {

ExprPtr ExpressionPlanner::plan(protocol::Expression const& proto_expr)
{
    switch (proto_expr.expr_type_case()) {
    case protocol::Expression::kColumnRef: {
        auto const& col_ref = proto_expr.column_ref();
        return std::make_shared<Expr>(ColumnRefExpr {
            col_ref.table(),
            col_ref.column() });
    }

    case protocol::Expression::kParameterRef: {
        auto const& param_ref = proto_expr.parameter_ref();
        return std::make_shared<Expr>(ParameterRefExpr {
            param_ref.parameter_index() });
    }

    case hegadb::protocol::Expression::kComparisonExpr: {
        auto const& comp = proto_expr.comparison_expr();
        return std::make_shared<Expr>(ComparisonExpr {
            plan(comp.left()),
            convertComparisonOp(comp.operator_()),
            plan(comp.right()) });
    }

    case hegadb::protocol::Expression::kLogicalExpr: {
        auto const& log = proto_expr.logical_expr();
        return std::make_shared<Expr>(LogicalExpr {
            plan(log.left()),
            convertLogicalOp(log.operator_()),
            plan(log.right()) });
    }

    case hegadb::protocol::Expression::kNotExpr: {
        auto const& not_expr = proto_expr.not_expr();
        return std::make_shared<Expr>(NotExpr {
            plan(not_expr.child()) });
    }

    case hegadb::protocol::Expression::kWildcard:
        throw std::runtime_error("Wildcard not supported in expression");

    default:
        throw std::runtime_error("Unknown expression type");
    }
}

operations::ComparisonOp ExpressionPlanner::convertComparisonOp(protocol::ComparisonOp op)
{
    switch (op) {
    case hegadb::protocol::COMPARISON_OP_EQ:
        return operations::ComparisonOp::EQ;
    case hegadb::protocol::COMPARISON_OP_NEQ:
        return operations::ComparisonOp::NEQ;
    case hegadb::protocol::COMPARISON_OP_LT:
        return operations::ComparisonOp::LT;
    case hegadb::protocol::COMPARISON_OP_LTE:
        return operations::ComparisonOp::LTE;
    case hegadb::protocol::COMPARISON_OP_GT:
        return operations::ComparisonOp::GT;
    case hegadb::protocol::COMPARISON_OP_GTE:
        return operations::ComparisonOp::GTE;

    default:
        throw std::runtime_error("Unknown comparison operator");
    }
}

operations::LogicalOp ExpressionPlanner::convertLogicalOp(protocol::LogicalOp op)
{
    switch (op) {
    case protocol::LOGICAL_OP_AND:
        return operations::LogicalOp::AND;
    case protocol::LOGICAL_OP_OR:
        return operations::LogicalOp::OR;
    default:
        throw std::runtime_error("Unknown logical operator");
    }
}

}
