#include <hegadb/server/evaluator/PredicateEvaluator.h>
#include <stdexcept>

namespace hegadb::server::evaluator {

PredicateEvaluator::PredicateEvaluator(execution::ExecutionContext& context)
    : context_(context)
    , comparison_evaluator_(context.getCryptoContext())
    , logical_evaluator_(context.getCryptoContext()) {
}

execution::EncryptedMask PredicateEvaluator::evaluate(
    planner::ExprPtr const& expr,
    execution::ExecutionBatch const& batch) {

    return std::visit([&](auto&& arg) -> execution::EncryptedMask {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, planner::ComparisonExpr>) {
            auto left_ct = evaluateValueExpr(arg.left, batch);
            auto right_ct = evaluateValueExpr(arg.right, batch);
            return comparison_evaluator_.evaluate(left_ct, arg.op, right_ct);
        }
        else if constexpr (std::is_same_v<T, planner::LogicalExpr>) {
            auto left_mask = evaluate(arg.left, batch);
            auto right_mask = evaluate(arg.right, batch);

            if (arg.op == operations::LogicalOp::AND) {
                return logical_evaluator_.evaluateAND(left_mask, right_mask);
            } else {
                return logical_evaluator_.evaluateOR(left_mask, right_mask);
            }
        }
        else if constexpr (std::is_same_v<T, planner::NotExpr>) {
            auto child_mask = evaluate(arg.child, batch);
            return logical_evaluator_.evaluateNOT(child_mask);
        }
        else {
            throw std::runtime_error("Invalid expression type for predicate");
        }
    }, *expr);
}

crypto::OpenFHECiphertext PredicateEvaluator::evaluateValueExpr(
    planner::ExprPtr const& expr,
    execution::ExecutionBatch const& batch) {

    return std::visit([&](auto&& arg) -> crypto::OpenFHECiphertext {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, planner::ColumnRefExpr>) {
            return batch.getColumn(arg.column);
        }
        else if constexpr (std::is_same_v<T, planner::ParameterRefExpr>) {
            return context_.getParameters().getParameter(arg.parameter_index);
        }
        else {
            throw std::runtime_error("Invalid value expression type");
        }
    }, *expr);
}

} // namespace hegadb::server::evaluator
