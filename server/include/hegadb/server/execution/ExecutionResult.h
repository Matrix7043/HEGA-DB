#ifndef EXECUTION_RESULT_H
#define EXECUTION_RESULT_H

#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include "query.pb.h"
#include <memory>
#include <vector>

namespace hegadb::server::execution {

/**
 * @brief Result of query execution
 *
 * Contains encrypted data and optionally an encrypted mask,
 * depending on the result mode.
 */
class ExecutionResult {
public:
    ExecutionResult() = default;

    /**
     * @brief Set the result mode
     */
    void setResultMode(protocol::ResultMode mode);

    /**
     * @brief Get the result mode
     */
    protocol::ResultMode getResultMode() const;

    /**
     * @brief Add a result batch
     */
    void addBatch(ExecutionBatch batch);

    /**
     * @brief Get all batches
     */
    std::vector<ExecutionBatch> const& getBatches() const;

    /**
     * @brief Set the encrypted mask (for ROWS_AND_MASK mode)
     */
    void setEncryptedMask(EncryptedMask mask);

    /**
     * @brief Get the encrypted mask
     * @throws std::runtime_error if no mask is set
     */
    EncryptedMask const& getEncryptedMask() const;

    /**
     * @brief Check if mask is present
     */
    bool hasMask() const;

    /**
     * @brief Get total row count across all batches
     */
    size_t getTotalRowCount() const;

private:
    protocol::ResultMode result_mode_ = protocol::RESULT_MODE_UNSPECIFIED;
    std::vector<ExecutionBatch> batches_;
    std::unique_ptr<EncryptedMask> encrypted_mask_;
};

} // namespace hegadb::server::execution

#endif // EXECUTION_RESULT_H
