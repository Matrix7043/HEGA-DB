#ifndef EXECUTION_PLAN_H
#define EXECUTION_PLAN_H

#include <hegadb/server/operators/Operator.h>
#include <memory>
#include <vector>

namespace hegadb::server::execution {

class ExecutionPlan {
public:
    ExecutionPlan() = default;

    void setRoot(std::unique_ptr<operators::Operator> root);

    operators::Operator& getRoot();
    operators::Operator const& getRoot() const;

    bool isValid() const;

private:
    std::unique_ptr<operators::Operator> root_;
};

}

#endif
