#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/ExecutionResult.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/transport/CatalogMutationClientTransport.h>
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
        std::shared_ptr<crypto::OpenFHEContext> crypto_context,
        std::shared_ptr<transport::CatalogMutationClientTransport> catalog_mutation = nullptr
    );

    /**
     * @brief Execute a query
     * @return Execution result with encrypted data
     */
    execution::ExecutionResult execute(protocol::ExecuteQuery const& request);

    /**
     * @brief Get active catalog reference
     */
    std::shared_ptr<catalog::Catalog const> getCatalog() const { return catalog_; }

private:
    execution::ExecutionResult executeCreateTable(protocol::CreateTableStatement const& stmt);
    execution::ExecutionResult executeInsert(protocol::InsertStatement const& stmt, protocol::ExecuteQuery const& request);
    void updateLocalCatalog(catalog::TableSchema const& schema);

    std::shared_ptr<catalog::Catalog const> catalog_;
    std::shared_ptr<storage::StorageManager> storage_;
    std::shared_ptr<crypto::OpenFHEContext> crypto_context_;
    std::shared_ptr<transport::CatalogMutationClientTransport> catalog_mutation_;
};

} // namespace hegadb::server

#endif // QUERY_EXECUTOR_H
