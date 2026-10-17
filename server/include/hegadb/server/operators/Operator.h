#ifndef OPERATOR_H
#define OPERATOR_H

#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/server/execution/ExecutionContext.h>
#include <optional>

namespace hegadb::server::operators {

class Operator {
public:
    virtual ~Operator() = default;

    virtual void open(execution::ExecutionContext& context) = 0;

    virtual std::optional<execution::ExecutionBatch> next() = 0;

    virtual void close() = 0;
};

} // namespace hegadb::server::operators

#endif // OPERATOR_H
