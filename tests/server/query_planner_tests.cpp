#include <gtest/gtest.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/server/planner/ExpressionPlanner.h>
#include <hegadb/server/planner/QueryPlanner.h>

using namespace hegadb::server::planner;
using namespace hegadb::catalog;
using namespace hegadb::protocol;
using namespace hegadb::types;

class QueryPlannerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // Create catalog with test table
        std::vector<ColumnSchema> columns = {
            { "id", DataType::Integer, false, 0 },
            { "name", DataType::String, false, 1 },
            { "age", DataType::Integer, false, 2 },
            { "salary", DataType::Integer, false, 3 }
        };

        auto table = std::make_shared<TableSchema>("employees", columns);

        // Create a catalog snapshot WITH the employees table
        std::unordered_map<std::string, TableSchema> tables;
        tables.emplace("employees", *table);
        auto snapshot = std::make_shared<CatalogSnapshot const>(1, std::move(tables));

        catalog_ = std::make_shared<InMemoryCatalog>(snapshot);
    }

    std::shared_ptr<Catalog> catalog_;
};

TEST_F(QueryPlannerTest, PlanSimpleSelectAll)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("employees");
    stmt.add_projections()->mutable_wildcard();

    auto plan = planner.plan(stmt);

    EXPECT_TRUE(plan.isValid());
}

TEST_F(QueryPlannerTest, PlanSelectWithSpecificColumns)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("employees");

    auto* proj1 = stmt.add_projections();
    proj1->mutable_column_ref()->set_column("id");

    auto* proj2 = stmt.add_projections();
    proj2->mutable_column_ref()->set_column("salary");

    auto plan = planner.plan(stmt);

    EXPECT_TRUE(plan.isValid());
}

TEST_F(QueryPlannerTest, PlanSelectWithWhere)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("employees");
    stmt.add_projections()->mutable_wildcard();

    // WHERE id = $0
    auto* where_expr = stmt.mutable_where();
    auto* comp = where_expr->mutable_comparison_expr();
    comp->set_operator_(COMPARISON_OP_EQ);
    comp->mutable_left()->mutable_column_ref()->set_column("id");
    comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);

    auto plan = planner.plan(stmt);

    EXPECT_TRUE(plan.isValid());
}

TEST_F(QueryPlannerTest, ThrowsOnUnknownTable)
{
    QueryPlanner planner(catalog_);

    SelectStatement stmt;
    stmt.mutable_from_table()->set_table("nonexistent");
    stmt.add_projections()->mutable_wildcard();

    EXPECT_THROW(planner.plan(stmt), std::runtime_error);
}
