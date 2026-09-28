#include <hegadb/server/operators/ProjectionOperator.h>
#include <hegadb/server/operators/FilterOperator.h>

namespace hegadb::server::operators {

ProjectionOperator::ProjectionOperator(
    std::unique_ptr<Operator> child,
    std::vector<std::string> column_names)
    : child_(std::move(child))
    , column_names_(std::move(column_names)) {
}

void ProjectionOperator::open(execution::ExecutionContext& context) {
    context_ = &context;
    child_->open(context);
}

std::optional<execution::ExecutionBatch> ProjectionOperator::next() {
    auto batch = child_->next();
    if (!batch) {
        return std::nullopt;
    }

    // If no specific columns requested, return all
    if (column_names_.empty()) {
        return batch;
    }

    // Create new batch with only requested columns
    execution::ExecutionBatch projected_batch;
    projected_batch.setRowCount(batch->getRowCount());

    for (auto const& col_name : column_names_) {
        if (batch->hasColumn(col_name)) {
            projected_batch.addColumn(col_name, batch->getColumn(col_name));
        }
    }

    return projected_batch;
}

void ProjectionOperator::close() {
    if (child_) {
        child_->close();
    }
}

FilterOperator* ProjectionOperator::getFilterChild() {
    return dynamic_cast<FilterOperator*>(child_.get());
}

} // namespace hegadb::server::operators
