#ifndef RESULT_STRATEGY_H
#define RESULT_STRATEGY_H

#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <vector>

namespace hegadb::server::result {

/**
 * @brief Strategy for constructing query results
 *
 * Different result modes:
 * - ROWS_AND_MASK: Return encrypted rows + encrypted mask
 * - MASKED_ROWS: Apply mask homomorphically to rows
 */
class ResultStrategy {
public:
    virtual ~ResultStrategy() = default;

    /**
     * @brief Build the execution result
     * @param batches Result batches
     * @param mask Encrypted predicate mask
     * @return Complete execution result
     */
    virtual execution::ExecutionResult buildResult(
        std::vector<execution::ExecutionBatch> batches,
        execution::EncryptedMask mask
    ) = 0;
};

} // namespace hegadb::server::result

#endif // RESULT_STRATEGY_H
