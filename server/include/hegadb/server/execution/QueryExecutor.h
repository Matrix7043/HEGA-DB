#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <hegadb/server/storage/StorageManager.h>
#include "query.pb.h"
#include <memory>

namespace hegadb::server {

/**
 * @brief Top-level query execution orchestrator
 *
 * Coordinates planning and execution of queries.
 */
class QueryExecutor {
public:
    QueryExecutor(
        std::shared_ptr<catalog::Catalog const> catalog,
        std::shared_ptr<storage::StorageManager> storage,
        std::shared_ptr<crypto::OpenFHEContext> crypto_context
    );

    /**
     * @brief Execute a query
     * @return Execution result with encrypted data
     */
    execution::ExecutionResult execute(protocol::ExecuteQuery const& request);

    using StreamingBatchConsumer = std::function<bool(execution::ExecutionBatch const&, execution::EncryptedMask const*, protocol::ResultMode)>;

    void executeStreaming(
        protocol::ExecuteQuery const& request,
        StreamingBatchConsumer consumer);

private:
    std::shared_ptr<catalog::Catalog const> catalog_;
    std::shared_ptr<storage::StorageManager> storage_;
    std::shared_ptr<crypto::OpenFHEContext> crypto_context_;
};

} // namespace hegadb::server

#endif // QUERY_EXECUTOR_H
