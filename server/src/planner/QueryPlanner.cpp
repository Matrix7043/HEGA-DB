#include <hegadb/server/operators/FilterOperator.h>
#include <hegadb/server/operators/JoinOperator.h>
#include <hegadb/server/operators/ProjectionOperator.h>
#include <hegadb/server/operators/TableScanOperator.h>
#include <hegadb/server/planner/ExpressionPlanner.h>
#include <hegadb/server/planner/QueryPlanner.h>
#include <stdexcept>

namespace hegadb::server::planner {

QueryPlanner::QueryPlanner(std::shared_ptr<catalog::Catalog const> catalog)
    : catalog_(std::move(catalog))
{
}

execution::ExecutionPlan QueryPlanner::plan(protocol::SelectStatement const& stmt)
{
    // Validate primary table exists
    auto const* table = catalog_->findTable(stmt.from_table().table());
    if (!table) {
        throw std::runtime_error("Table not found: " + stmt.from_table().table());
    }

    // 1. Base table scan for left table
    std::unique_ptr<operators::Operator> current =
        std::make_unique<operators::TableScanOperator>(stmt.from_table().table());

    // 2. Handle JOIN if present
    if (stmt.joins_size() > 0) {
        auto const& join_spec = stmt.joins(0);
        auto const& right_table_name = join_spec.table().table();

        auto const* right_table = catalog_->findTable(right_table_name);
        if (!right_table) {
            throw std::runtime_error("Joined table not found: " + right_table_name);
        }

        auto right_scan = std::make_unique<operators::TableScanOperator>(right_table_name);
        auto join_condition = ExpressionPlanner::plan(join_spec.condition());

        current = std::make_unique<operators::JoinOperator>(
            std::move(current),
            std::move(right_scan),
            stmt.from_table().table(),
            right_table_name,
            join_condition
        );
    }

    // 3. Filter (WHERE clause)
    if (stmt.has_where()) {
        auto predicate = ExpressionPlanner::plan(stmt.where());
        current = std::make_unique<operators::FilterOperator>(
            std::move(current),
            predicate);
    }

    // 4. Projection
    auto projection_columns = extractProjectionColumns(stmt);
    current = std::make_unique<operators::ProjectionOperator>(
        std::move(current),
        projection_columns);

    execution::ExecutionPlan plan;
    plan.setRoot(std::move(current));
    return plan;
}

std::vector<std::string> QueryPlanner::extractProjectionColumns(
    protocol::SelectStatement const& stmt)
{
    std::vector<std::string> columns;

    for (auto const& proj : stmt.projections()) {
        if (proj.has_wildcard()) {
            return { };
        } else if (proj.has_column_ref()) {
            auto const& col = proj.column_ref();
            if (!col.table().empty()) {
                columns.push_back(col.table() + "." + col.column());
            } else {
                columns.push_back(col.column());
            }
        } else {
            throw std::runtime_error("Unsupported projection expression");
        }
    }

    return columns;
}

} // namespace hegadb::server::planner
