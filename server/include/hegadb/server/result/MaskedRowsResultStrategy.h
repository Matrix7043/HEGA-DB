#ifndef MASKED_ROWS_RESULT_STRATEGY_H
#define MASKED_ROWS_RESULT_STRATEGY_H

#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/result/ResultStrategy.h>

namespace hegadb::server::result {

/**
 * Result strategy that applies mask to rows server-side
 *
 * Homomorphically multiplies encrypted mask with encrypted values:
 * C(mask) × C(value) = C(masked_value)
 */
class MaskedRowsResultStrategy : public ResultStrategy {
public:
    explicit MaskedRowsResultStrategy(crypto::OpenFHEContext& crypto_context);

    execution::ExecutionResult buildResult(
        std::vector<execution::ExecutionBatch> batches,
        execution::EncryptedMask mask
    ) override;

private:
    crypto::OpenFHEContext& crypto_context_;
};

} // namespace hegadb::server::result

#endif // MASKED_ROWS_RESULT_STRATEGY_H
