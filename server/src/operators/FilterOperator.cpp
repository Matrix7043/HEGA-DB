#include <hegadb/server/evaluator/PredicateEvaluator.h>
#include <hegadb/server/operators/FilterOperator.h>
#include <stdexcept>

namespace hegadb::server::operators {

FilterOperator::FilterOperator(
    std::unique_ptr<Operator> child,
    planner::ExprPtr predicate)
    : child_(std::move(child))
    , predicate_(std::move(predicate))
{
}

void FilterOperator::open(execution::ExecutionContext& context)
{
    context_ = &context;
    child_->open(context);
}

std::optional<execution::ExecutionBatch> FilterOperator::next()
{
    if (!context_) {
        return std::nullopt;
    }

    auto batch = child_->next();
    if (!batch) {
        return std::nullopt;
    }

    // Evaluate predicate on this batch
    evaluator::PredicateEvaluator pred_eval(*context_);
    auto mask = pred_eval.evaluate(predicate_, *batch);

    // Accumulate mask across batches.
    // Each batch from a paged source occupies non-overlapping SIMD slot ranges,
    // so OR (implemented as homomorphic EvalAdd on 0/1 values) correctly merges them:
    // a slot that is 1 in any batch becomes 1 in the combined mask.
    if (!accumulated_mask_) {
        accumulated_mask_ = std::make_unique<execution::EncryptedMask>(std::move(mask));
    } else {
        evaluator::LogicalEvaluator logical_eval(context_->getCryptoContext());
        auto combined = logical_eval.evaluateOR(*accumulated_mask_, mask);
        accumulated_mask_ = std::make_unique<execution::EncryptedMask>(std::move(combined));
    }

    // Pass through the batch unchanged
    // Result strategy will handle mask application
    return batch;
}

void FilterOperator::close()
{
    if (child_) {
        child_->close();
    }
}

execution::EncryptedMask FilterOperator::getAccumulatedMask() const
{
    if (!accumulated_mask_) {
        throw std::runtime_error("No accumulated mask available");
    }
    return *accumulated_mask_;
}

} // namespace hegadb::server::operators
