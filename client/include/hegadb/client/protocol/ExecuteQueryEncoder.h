#ifndef EXECUTE_QUERY_ENCODER
#define EXECUTE_QUERY_ENCODER

#include "hegadb/client/preparation/PreparedQuery.h"
#include "query.pb.h"

namespace hegadb::client::protocol {

class ExecuteQueryEncoder {

public:
    static hegadb::protocol::ExecuteQuery encode(
        preparation::PreparedQuery const& prepared,
        std::string request_id);
};

}

#endif
