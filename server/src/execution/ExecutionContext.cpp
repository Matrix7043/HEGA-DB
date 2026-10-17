#include <hegadb/server/execution/ExecutionContext.h>

namespace hegadb::server::execution {

ExecutionContext::ExecutionContext(
    std::shared_ptr<catalog::Catalog const> catalog,
    std::shared_ptr<storage::StorageManager> storage,
    std::shared_ptr<crypto::OpenFHEContext> crypto_context,
    protocol::ResultMode result_mode)
    : catalog_(std::move(catalog))
    , storage_(std::move(storage))
    , crypto_context_(std::move(crypto_context))
    , result_mode_(result_mode) {
}

catalog::Catalog const& ExecutionContext::getCatalog() const {
    return *catalog_;
}

storage::StorageManager& ExecutionContext::getStorageManager() const {
    return *storage_;
}

crypto::OpenFHEContext& ExecutionContext::getCryptoContext() const {
    return *crypto_context_;
}

ExecutionParameters& ExecutionContext::getParameters() {
    return parameters_;
}

ExecutionParameters const& ExecutionContext::getParameters() const {
    return parameters_;
}

protocol::ResultMode ExecutionContext::getResultMode() const {
    return result_mode_;
}

void ExecutionContext::setRequestId(std::string const& request_id) {
    request_id_ = request_id;
}

std::string const& ExecutionContext::getRequestId() const {
    return request_id_;
}

} // namespace hegadb::server::execution
