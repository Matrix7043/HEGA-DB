#include <hegadb/server/operators/TableScanOperator.h>

namespace hegadb::server::operators {

TableScanOperator::TableScanOperator(std::string table_name)
    : table_name_(std::move(table_name)) {
}

void TableScanOperator::open(execution::ExecutionContext& context) {
    context_ = &context;
    table_ = context.getStorageManager().openTable(table_name_);
    table_->open();
}

std::optional<execution::ExecutionBatch> TableScanOperator::next() {
    if (!table_) {
        return std::nullopt;
    }

    auto storage_batch = table_->nextBatch();
    if (!storage_batch) {
        return std::nullopt;
    }

    // Convert storage batch to execution batch
    execution::ExecutionBatch exec_batch;
    exec_batch.setRowCount(storage_batch->row_count);

    for (auto& [col_name, ciphertext] : storage_batch->columns) {
        exec_batch.addColumn(col_name, std::move(ciphertext));
    }

    return exec_batch;
}

void TableScanOperator::close() {
    if (table_) {
        table_->close();
        table_.reset();
    }
}

} // namespace hegadb::server::operators

