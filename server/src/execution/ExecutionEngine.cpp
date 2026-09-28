#include <hegadb/server/execution/ExecutionEngine.h>
#include <hegadb/server/operators/FilterOperator.h>
#include <hegadb/server/operators/ProjectionOperator.h>
#include <hegadb/server/result/MaskedRowsResultStrategy.h>
#include <hegadb/server/result/RowsAndMaskResultStrategy.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/types/DataType.h>
#include <stdexcept>

namespace hegadb::server::execution {

ExecutionResult ExecutionEngine::execute(
    ExecutionPlan& plan,
    ExecutionContext& context)
{

    if (!plan.isValid()) {
        throw std::runtime_error("Invalid execution plan");
    }

    auto& root = plan.getRoot();
    
    // Try to identify if we have a FilterOperator in the plan
    // The typical structure is: ProjectionOperator -> FilterOperator -> TableScanOperator
    operators::FilterOperator* filter_op = nullptr;
    
    // Check if root is a ProjectionOperator (most common case)
    if (auto* proj_op = dynamic_cast<operators::ProjectionOperator*>(&root)) {
        // ProjectionOperator has a child - check if it's a FilterOperator
        filter_op = proj_op->getFilterChild();
    } else {
        // Root might be FilterOperator directly (less common)
        filter_op = dynamic_cast<operators::FilterOperator*>(&root);
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
    
    // Extract mask from FilterOperator if present
    if (filter_op != nullptr) {
        try {
            auto mask = filter_op->getAccumulatedMask();
            result.setEncryptedMask(std::move(mask));
        } catch (std::exception const& e) {
            // FilterOperator may not have a mask if no data was processed
            // This is fine - proceed without mask
        }
    }
    
    // If no filter exists, we don't set a mask (all rows implicitly pass)

    return result;
}

} // namespace hegadb::server::execution
