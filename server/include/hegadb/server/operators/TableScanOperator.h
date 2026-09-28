#ifndef TABLE_SCAN_OPERATOR_H
#define TABLE_SCAN_OPERATOR_H

#include <hegadb/server/operators/Operator.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <memory>
#include <string>

namespace hegadb::server::operators {

class TableScanOperator : public Operator {
public:
    explicit TableScanOperator(std::string table_name);

    void open(execution::ExecutionContext& context) override;
    std::optional<execution::ExecutionBatch> next() override;
    void close() override;

private:
    std::string table_name_;
    execution::ExecutionContext* context_ = nullptr;
    std::unique_ptr<storage::EncryptedTable> table_;
};

} // namespace hegadb::server::operators

#endif // TABLE_SCAN_OPERATOR_H

