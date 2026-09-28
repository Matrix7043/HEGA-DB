#ifndef PROJECTION_OPERATOR_H
#define PROJECTION_OPERATOR_H

#include <hegadb/server/operators/Operator.h>
#include <string>
#include <vector>

namespace hegadb::server::operators {

// Forward declaration
class FilterOperator;

/**
 * @brief Operator that projects selected columns
 *
 * Filters the input batch to include only requested columns.
 */
class ProjectionOperator : public Operator {
public:
    /**
     * @brief Construct a projection operator
     * @param child Child operator
     * @param column_names Columns to project (empty = all columns)
     */
    ProjectionOperator(
        std::unique_ptr<Operator> child,
        std::vector<std::string> column_names
    );

    void open(execution::ExecutionContext& context) override;
    std::optional<execution::ExecutionBatch> next() override;
    void close() override;

    /**
     * @brief Get the child operator as a FilterOperator if it is one
     * @return Pointer to FilterOperator child, or nullptr if child is not a filter
     */
    FilterOperator* getFilterChild();

private:
    std::unique_ptr<Operator> child_;
    std::vector<std::string> column_names_;
    execution::ExecutionContext* context_ = nullptr;
};

} // namespace hegadb::server::operators

#endif // PROJECTION_OPERATOR_H
