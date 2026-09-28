#include <hegadb/server/planner/ExpressionPlanner.h>
#include <gtest/gtest.h>

using namespace hegadb::server::planner;
using namespace hegadb::protocol;

TEST(ExpressionPlannerTest, PlanColumnRef) {
    Expression proto_expr;
    auto* col_ref = proto_expr.mutable_column_ref();
    col_ref->set_table("employees");
    col_ref->set_column("id");

    auto expr = ExpressionPlanner::plan(proto_expr);

    ASSERT_TRUE(std::holds_alternative<hegadb::server::planner::ColumnRefExpr>(*expr));
    auto const& col = std::get<hegadb::server::planner::ColumnRefExpr>(*expr);
    EXPECT_EQ(col.table, "employees");
    EXPECT_EQ(col.column, "id");
}

TEST(ExpressionPlannerTest, PlanParameterRef) {
    Expression proto_expr;
    auto* param_ref = proto_expr.mutable_parameter_ref();
    param_ref->set_parameter_index(0);

    auto expr = ExpressionPlanner::plan(proto_expr);

    ASSERT_TRUE(std::holds_alternative<hegadb::server::planner::ParameterRefExpr>(*expr));
    auto const& param = std::get<hegadb::server::planner::ParameterRefExpr>(*expr);
    EXPECT_EQ(param.parameter_index, 0);
}

TEST(ExpressionPlannerTest, PlanComparisonExpr) {
    Expression proto_expr;
    auto* comp = proto_expr.mutable_comparison_expr();
    comp->set_operator_(COMPARISON_OP_EQ);
    comp->mutable_left()->mutable_column_ref()->set_column("id");
    comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);

    auto expr = ExpressionPlanner::plan(proto_expr);

    ASSERT_TRUE(std::holds_alternative<hegadb::server::planner::ComparisonExpr>(*expr));
    auto const& comparison = std::get<hegadb::server::planner::ComparisonExpr>(*expr);
    EXPECT_EQ(comparison.op, hegadb::server::planner::ComparisonOp::EQ);
}

TEST(ExpressionPlannerTest, PlanLogicalExpr) {
    Expression proto_expr;
    auto* logical = proto_expr.mutable_logical_expr();
    logical->set_operator_(LOGICAL_OP_AND);

    // Left: id = $0
    auto* left_comp = logical->mutable_left()->mutable_comparison_expr();
    left_comp->set_operator_(COMPARISON_OP_EQ);
    left_comp->mutable_left()->mutable_column_ref()->set_column("id");
    left_comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);

    // Right: age > $1
    auto* right_comp = logical->mutable_right()->mutable_comparison_expr();
    right_comp->set_operator_(COMPARISON_OP_GT);
    right_comp->mutable_left()->mutable_column_ref()->set_column("age");
    right_comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(1);

    auto expr = ExpressionPlanner::plan(proto_expr);

    ASSERT_TRUE(std::holds_alternative<hegadb::server::planner::LogicalExpr>(*expr));
    auto const& log_expr = std::get<hegadb::server::planner::LogicalExpr>(*expr);
    EXPECT_EQ(log_expr.op, hegadb::server::planner::LogicalOp::AND);
}

TEST(ExpressionPlannerTest, PlanNotExpr) {
    Expression proto_expr;
    auto* not_expr = proto_expr.mutable_not_expr();
    not_expr->mutable_child()->mutable_column_ref()->set_column("active");

    auto expr = ExpressionPlanner::plan(proto_expr);

    ASSERT_TRUE(std::holds_alternative<hegadb::server::planner::NotExpr>(*expr));
}

TEST(ExpressionPlannerTest, WildcardThrows) {
    Expression proto_expr;
    proto_expr.mutable_wildcard();

    EXPECT_THROW(ExpressionPlanner::plan(proto_expr), std::runtime_error);
}

TEST(ExpressionPlannerTest, ConvertComparisonOperators) {
    EXPECT_NO_THROW({
        Expression expr;
        auto* comp = expr.mutable_comparison_expr();
        comp->set_operator_(COMPARISON_OP_EQ);
        comp->mutable_left()->mutable_column_ref()->set_column("id");
        comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);
        ExpressionPlanner::plan(expr);
    });

    EXPECT_NO_THROW({
        Expression expr;
        auto* comp = expr.mutable_comparison_expr();
        comp->set_operator_(COMPARISON_OP_NEQ);
        comp->mutable_left()->mutable_column_ref()->set_column("id");
        comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);
        ExpressionPlanner::plan(expr);
    });

    EXPECT_NO_THROW({
        Expression expr;
        auto* comp = expr.mutable_comparison_expr();
        comp->set_operator_(COMPARISON_OP_LT);
        comp->mutable_left()->mutable_column_ref()->set_column("id");
        comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);
        ExpressionPlanner::plan(expr);
    });
}
