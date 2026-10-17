#include <hegadb/server/execution/ExecutionResult.h>
#include <stdexcept>

namespace hegadb::server::execution {

void ExecutionResult::setResultMode(protocol::ResultMode mode) {
    result_mode_ = mode;
}

protocol::ResultMode ExecutionResult::getResultMode() const {
    return result_mode_;
}

void ExecutionResult::addBatch(ExecutionBatch batch) {
    batches_.push_back(std::move(batch));
}

std::vector<ExecutionBatch> const& ExecutionResult::getBatches() const {
    return batches_;
}

void ExecutionResult::setEncryptedMask(EncryptedMask mask) {
    encrypted_mask_ = std::make_unique<EncryptedMask>(std::move(mask));
}

EncryptedMask const& ExecutionResult::getEncryptedMask() const {
    if (!encrypted_mask_) {
        throw std::runtime_error("No encrypted mask set in execution result");
    }
    return *encrypted_mask_;
}

bool ExecutionResult::hasMask() const {
    return encrypted_mask_ != nullptr;
}

size_t ExecutionResult::getTotalRowCount() const {
    size_t total = 0;
    for (auto const& batch : batches_) {
        total += batch.getRowCount();
    }
    return total;
}

} // namespace hegadb::server::execution
