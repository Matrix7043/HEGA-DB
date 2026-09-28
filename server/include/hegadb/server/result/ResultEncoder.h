#ifndef RESULT_ENCODER_H
#define RESULT_ENCODER_H

#include <hegadb/server/execution/ExecutionResult.h>
#include "query.pb.h"

namespace hegadb::server::result {

class ResultEncoder {
public:

    static void encode(
        execution::ExecutionResult const& exec_result,
        protocol::QueryResponse& response
    );
};

} // namespace hegadb::server::result

#endif // RESULT_ENCODER_H
