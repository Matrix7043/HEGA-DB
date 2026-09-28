#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/ExecutionParameters.h>
#include <hegadb/server/storage/StorageManager.h>
#include "query.pb.h"
#include <memory>
#include <string>

namespace hegadb::server::execution {

/**
 * @brief Runtime context for query execution
 *
 * Provides access to all resources needed during query execution:
 * - Catalog for schema information
 * - Storage manager for data access
 * - Crypto context for FHE operations
 * - Encrypted parameters
 * - Result mode configuration
 */
class ExecutionContext {
public:
    ExecutionContext(
        std::shared_ptr<catalog::Catalog const> catalog,
        std::shared_ptr<storage::StorageManager> storage,
        std::shared_ptr<crypto::OpenFHEContext> crypto_context,
        protocol::ResultMode result_mode
    );

    // Accessors
    catalog::Catalog const& getCatalog() const;
    storage::StorageManager& getStorageManager() const;
    crypto::OpenFHEContext& getCryptoContext() const;
    ExecutionParameters& getParameters();
    ExecutionParameters const& getParameters() const;
    protocol::ResultMode getResultMode() const;

    // Request information
    void setRequestId(std::string const& request_id);
    std::string const& getRequestId() const;

private:
    std::shared_ptr<catalog::Catalog const> catalog_;
    std::shared_ptr<storage::StorageManager> storage_;
    std::shared_ptr<crypto::OpenFHEContext> crypto_context_;
    ExecutionParameters parameters_;
    protocol::ResultMode result_mode_;
    std::string request_id_;
};

} // namespace hegadb::server::execution

#endif // EXECUTION_CONTEXT_H
