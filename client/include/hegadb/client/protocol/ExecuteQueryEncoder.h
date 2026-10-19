#ifndef EXECUTE_QUERY_ENCODER
#define EXECUTE_QUERY_ENCODER

#include "hegadb/client/ast/CreateTableStmt.h"
#include "hegadb/client/ast/DropTableStmt.h"
#include "hegadb/client/ast/Statement.h"
#include "hegadb/client/preparation/PreparedQuery.h"
#include "query.pb.h"

#include <string>

namespace hegadb::client::protocol {

class ExecuteQueryEncoder {

public:
    // Encodes a prepared SELECT or INSERT. An INSERT carries its values as
    // encrypted parameters, like the literals of a WHERE clause, so it must go
    // through query preparation first. Throws std::invalid_argument for any
    // other statement kind.
    static hegadb::protocol::ExecuteQuery encode(
        preparation::PreparedQuery const& prepared,
        std::string request_id);

    // CREATE TABLE / DROP TABLE carry no literals, so they skip query
    // preparation: no encrypted parameters and no public keys are sent.
    static hegadb::protocol::ExecuteQuery encode(
        ast::CreateTableStmt const& stmt,
        std::string request_id);

    static hegadb::protocol::ExecuteQuery encode(
        ast::DropTableStmt const& stmt,
        std::string request_id);

    // Dispatches to one of the two overloads above based on stmt.kind().
    // Throws std::invalid_argument for any other statement kind
    // (SELECT must go through query preparation).
    static hegadb::protocol::ExecuteQuery encodeDdl(
        ast::Statement const& stmt,
        std::string request_id);
};

}

#endif
