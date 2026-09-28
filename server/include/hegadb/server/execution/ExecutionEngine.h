#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include <hegadb/server/execution/ExecutionContext.h>
#include <hegadb/server/execution/ExecutionPlan.h>
#include <hegadb/server/execution/ExecutionResult.h>

namespace hegadb::server::execution {

/**
 * @brief Executes physical query plans
 *
 * Runs the operator tree and applies result strategy.
 */
class ExecutionEngine {
public:
    /**
     * @brief Execute a query plan
     * @param plan Physical execution plan
     * @param context Execution context
     * @return Execution result
     */
    ExecutionResult execute(
        ExecutionPlan& plan,
        ExecutionContext& context);

private:
    /**
     * @brief Extract encrypted mask from plan
     * @param plan Physical execution plan
     * @param context Execution context (for crypto context)
     * @param row_count Total number of rows in result
     * @return Encrypted mask
     */
    EncryptedMask extractMaskFromPlan(
        ExecutionPlan& plan,
        ExecutionContext& context,
        size_t row_count);
};

} // namespace hegadb::server::execution

#endif // EXECUTION_ENGINE_H
