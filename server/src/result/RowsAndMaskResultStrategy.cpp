#include <hegadb/server/result/RowsAndMaskResultStrategy.h>

namespace hegadb::server::result {

execution::ExecutionResult RowsAndMaskResultStrategy::buildResult(
    std::vector<execution::ExecutionBatch> batches,
    execution::EncryptedMask mask) {

    execution::ExecutionResult result;
    result.setResultMode(protocol::RESULT_MODE_ROWS_AND_MASK);
    result.setEncryptedMask(std::move(mask));

    for (auto& batch : batches) {
        result.addBatch(std::move(batch));
    }

    return result;
}

} // namespace hegadb::server::result
