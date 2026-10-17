#include <hegadb/server/execution/ExecutionPlan.h>
#include <stdexcept>

namespace hegadb::server::execution {

void ExecutionPlan::setRoot(std::unique_ptr<operators::Operator> root)
{
    root_ = std::move(root);
}

operators::Operator& ExecutionPlan::getRoot()
{
    if (!root_) {
        throw std::runtime_error("Execution plan has no root operator");
    }
    return *root_;
}

operators::Operator const& ExecutionPlan::getRoot() const
{
    if (!root_) {
        throw std::runtime_error("Execution plan has no root operator");
    }
    return *root_;
}

bool ExecutionPlan::isValid() const
{
    return root_ != nullptr;
}

} // namespace hegadb::server::execution
