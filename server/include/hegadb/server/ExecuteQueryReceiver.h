#ifndef EXECUTE_QUERY_RECEIVER_H
#define EXECUTE_QUERY_RECEIVER_H

#include <hegadb/catalog/Catalog.h>
#include <hegadb/server/QueryExecutor.h>

#include "common.pb.h"
#include "query.pb.h"

#include <memory>
// #include <tl/expected.hpp>

namespace hegadb::server {

class ExecuteQueryReceiver {
public:
    explicit ExecuteQueryReceiver(
        // std::shared_ptr<catalog::Catalog const> catalog,
        // std::shared_ptr<QueryExecutor> executor,
    );

    // ~ExecuteQueryReceiver();

    // Recieves and executes the given query
    protocol::QueryResponse executeQuery(
        protocol::ExecuteQuery const& request);

private:
    bool validateProtocolVersion(
        protocol::ProtocolVersion const& version,
        std::string& error_message) const;

    protocol::QueryResponse createResponse(
        protocol::ProtocolVersion const& version,
        protocol::RequestId const& request_id,
        protocol::StatusCode status,
        std::string message) const;

private:
    std::shared_ptr<catalog::Catalog const> catalog_;
    std::shared_ptr<QueryExecutor> executor_;
};

} // namespace hegadb::server

#endif // EXECUTE_QUERY_RECEIVER_H
