#ifndef ROWS_AND_MASK_RESULT_STRATEGY_H
#define ROWS_AND_MASK_RESULT_STRATEGY_H

#include <hegadb/server/result/ResultStrategy.h>

namespace hegadb::server::result {

/**
 * @brief Result strategy that returns rows and mask separately
 *
 * Client receives both encrypted data and encrypted mask,
 * then decrypts and interprets on client side.
 */
class RowsAndMaskResultStrategy : public ResultStrategy {
public:
    execution::ExecutionResult buildResult(
        std::vector<execution::ExecutionBatch> batches,
        execution::EncryptedMask mask
    ) override;
};

} // namespace hegadb::server::result

#endif // ROWS_AND_MASK_RESULT_STRATEGY_H
