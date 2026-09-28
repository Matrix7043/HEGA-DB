#ifndef FILTER_OPERATOR_H
#define FILTER_OPERATOR_H

#include <hegadb/server/operators/Operator.h>
#include <hegadb/server/planner/ExpressionPlanner.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <memory>

namespace hegadb::server::operators {

/**
 * @brief Operator that applies a predicate filter
 *
 * Evaluates the WHERE clause on encrypted data,
 * producing an encrypted mask.
 */
class FilterOperator : public Operator {
public:
    /**
     * @brief Construct a filter operator
     * @param child Child operator producing input batches
     * @param predicate Predicate expression to evaluate
     */
    FilterOperator(
        std::unique_ptr<Operator> child,
        planner::ExprPtr predicate
    );

    void open(execution::ExecutionContext& context) override;
    std::optional<execution::ExecutionBatch> next() override;
    void close() override;

    /**
     * @brief Get the accumulated encrypted mask
     *
     * After all batches are processed, returns the combined mask.
     */
    execution::EncryptedMask getAccumulatedMask() const;

private:
    std::unique_ptr<Operator> child_;
    planner::ExprPtr predicate_;
    execution::ExecutionContext* context_ = nullptr;
    std::unique_ptr<execution::EncryptedMask> accumulated_mask_;
};

} // namespace hegadb::server::operators

#endif // FILTER_OPERATOR_H
