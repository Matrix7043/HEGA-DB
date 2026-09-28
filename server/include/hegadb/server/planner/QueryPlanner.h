#ifndef QUERY_PLANNER_H
#define QUERY_PLANNER_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/server/execution/ExecutionPlan.h>
#include "query.pb.h"
#include <memory>

namespace hegadb::server::planner {

/**
 * @brief Converts protocol AST to physical execution plan
 *
 * Creates operator tree for query execution.
 */
class QueryPlanner {
public:
    explicit QueryPlanner(std::shared_ptr<catalog::Catalog const> catalog);

    /**
     * @brief Create execution plan from SELECT statement
     */
    execution::ExecutionPlan plan(protocol::SelectStatement const& stmt);

private:
    std::vector<std::string> extractProjectionColumns(
        protocol::SelectStatement const& stmt
    );

    std::shared_ptr<catalog::Catalog const> catalog_;
};

} // namespace hegadb::server::planner

#endif // QUERY_PLANNER_H
