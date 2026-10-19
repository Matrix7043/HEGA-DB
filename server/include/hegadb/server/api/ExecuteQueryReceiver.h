#ifndef EXECUTE_QUERY_RECEIVER_H
#define EXECUTE_QUERY_RECEIVER_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/server/execution/QueryExecutor.h>

#include "common.pb.h"
#include "query.pb.h"

#include <memory>
#include <string>

namespace hegadb::server {

class ExecuteQueryReceiver {
public:
    explicit ExecuteQueryReceiver(
        std::shared_ptr<catalog::Catalog const> catalog,
        std::shared_ptr<QueryExecutor> executor
    );

    protocol::QueryResponse executeQuery(
        protocol::ExecuteQuery const& request
    );

    using BatchWriter = std::function<bool(protocol::QueryResponse const&)>;

    void executeQueryStream(
        protocol::ExecuteQuery const& request,
        BatchWriter writer
    );

private:
    bool validateProtocolVersion(
        protocol::ProtocolVersion const& version,
        std::string& error_message
    ) const;

    bool validateRequest(
        protocol::ExecuteQuery const& request,
        std::string& error_message
    ) const;

    protocol::QueryResponse createResponse(
        protocol::ProtocolVersion const& version,
        protocol::RequestId const& request_id,
        protocol::StatusCode status,
        std::string message
    ) const;

    protocol::QueryResponse createErrorResponse(
        protocol::ProtocolVersion const& version,
        protocol::RequestId const& request_id,
        std::string const& error_message
    ) const;

private:
    std::shared_ptr<catalog::Catalog const> catalog_;
    std::shared_ptr<QueryExecutor> executor_;

    // Protocol version constraints
    static constexpr uint32_t SUPPORTED_MAJOR_VERSION = 1;
    static constexpr uint32_t MIN_MINOR_VERSION = 0;
};

} // namespace hegadb::server

#endif // EXECUTE_QUERY_RECEIVER_H
