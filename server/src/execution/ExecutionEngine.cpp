#include <hegadb/server/execution/ExecutionEngine.h>
#include <hegadb/server/operators/FilterOperator.h>
#include <hegadb/server/operators/JoinOperator.h>
#include <hegadb/server/operators/ProjectionOperator.h>
#include <hegadb/server/evaluator/LogicalEvaluator.h>
#include <hegadb/server/result/MaskedRowsResultStrategy.h>
#include <hegadb/server/result/RowsAndMaskResultStrategy.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/types/DataType.h>
#include <stdexcept>
#include <optional>

namespace hegadb::server::execution {

ExecutionResult ExecutionEngine::execute(
    ExecutionPlan& plan,
    ExecutionContext& context)
{

    if (!plan.isValid()) {
        throw std::runtime_error("Invalid execution plan");
    }

    auto& root = plan.getRoot();
    
    // Identify operators in the plan
    operators::FilterOperator* filter_op = nullptr;
    operators::JoinOperator* join_op = nullptr;

    operators::Operator* current = &root;
    if (auto* proj_op = dynamic_cast<operators::ProjectionOperator*>(current)) {
        current = proj_op->getChild();
    }
    if (auto* f_op = dynamic_cast<operators::FilterOperator*>(current)) {
        filter_op = f_op;
        current = f_op->getChild();
    }
    if (auto* j_op = dynamic_cast<operators::JoinOperator*>(current)) {
        join_op = j_op;
    }
    
    // Execute the plan
    root.open(context);

    // Collect all result batches
    std::vector<ExecutionBatch> batches;
    while (auto batch = root.next()) {
        batches.push_back(std::move(*batch));
    }

    root.close();

    // Build result
    ExecutionResult result;
    result.setResultMode(context.getResultMode());
    
    for (auto& batch : batches) {
        result.addBatch(std::move(batch));
    }
    
    // Extract and combine masks if present
    std::optional<EncryptedMask> final_mask;

    if (join_op != nullptr) {
        try {
            final_mask = join_op->getAccumulatedMask();
        } catch (std::exception const& e) {
            // JoinOperator may not have a mask if no data was processed
        }
    }

    if (filter_op != nullptr) {
        try {
            auto f_mask = filter_op->getAccumulatedMask();
            if (final_mask.has_value()) {
                evaluator::LogicalEvaluator logical_eval(context.getCryptoContext());
                final_mask = logical_eval.evaluateAND(*final_mask, f_mask);
            } else {
                final_mask = std::move(f_mask);
            }
        } catch (std::exception const& e) {
            // FilterOperator may not have a mask if no data was processed
        }
    }

    if (final_mask.has_value()) {
        result.setEncryptedMask(std::move(*final_mask));
    }

    return result;
}

} // namespace hegadb::server::execution
