#include <hegadb/server/operators/FilterOperator.h>
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
    // Validate table exists
    auto const* table = catalog_->findTable(stmt.from_table().table());
    if (!table) {
        throw std::runtime_error("Table not found: " + stmt.from_table().table());
    }

    // Build operator tree: Projection -> Filter -> TableScan

    // 1. Table scan
    auto scan = std::make_unique<operators::TableScanOperator>(
        stmt.from_table().table());

    // 2. Filter (if WHERE clause exists)
    std::unique_ptr<operators::Operator> current = std::move(scan);

    if (stmt.has_where()) {
        auto predicate = ExpressionPlanner::plan(stmt.where());
        current = std::make_unique<operators::FilterOperator>(
            std::move(current),
            predicate);
    }

    // 3. Projection
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
            // SELECT * - return empty to indicate all columns
            return { };
        } else if (proj.has_column_ref()) {
            columns.push_back(proj.column_ref().column());
        } else {
            throw std::runtime_error("Unsupported projection expression");
        }
    }

    return columns;
}

} // namespace hegadb::server::planner
