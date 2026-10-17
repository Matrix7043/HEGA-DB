#ifndef PREDICATE_EVALUATOR_H
#define PREDICATE_EVALUATOR_H

#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <hegadb/server/evaluator/LogicalEvaluator.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/planner/ExpressionPlanner.h>

namespace hegadb::server::evaluator {

/**
 * @brief Evaluates predicate expressions on encrypted data
 *
 * Recursively evaluates expression trees, combining comparison
 * and logical operators to produce an encrypted mask.
 */
class PredicateEvaluator {
public:
    explicit PredicateEvaluator(execution::ExecutionContext& context);

    /**
     * @brief Evaluate a predicate expression on a batch
     * @param expr Expression to evaluate
     * @param batch Data batch
     * @return Encrypted mask representing predicate result
     */
    execution::EncryptedMask evaluate(
        planner::ExprPtr const& expr,
        execution::ExecutionBatch const& batch
    );

private:
    crypto::OpenFHECiphertext evaluateValueExpr(
        planner::ExprPtr const& expr,
        execution::ExecutionBatch const& batch
    );

    execution::ExecutionContext& context_;
    ComparisonEvaluator comparison_evaluator_;
    LogicalEvaluator logical_evaluator_;
};

} // namespace hegadb::server::evaluator

#endif // PREDICATE_EVALUATOR_H
