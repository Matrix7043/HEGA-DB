#ifndef JOIN_OPERATOR_H
#define JOIN_OPERATOR_H

#include <hegadb/server/operators/Operator.h>
#include <hegadb/server/planner/ExpressionPlanner.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <memory>
#include <string>

namespace hegadb::server::operators {

/**
 * @brief Operator that performs homomorphic equi-joins between two tables
 *
 * Takes a left child (outer) and right child (inner), evaluates the join condition
 * across row pairs homomorphically, and emits an ExecutionBatch of size (N * M)
 * containing columns from both tables along with the computed EncryptedMask.
 */
class JoinOperator : public Operator {
public:
    JoinOperator(
        std::unique_ptr<Operator> left_child,
        std::unique_ptr<Operator> right_child,
        std::string left_table_name,
        std::string right_table_name,
        planner::ExprPtr join_condition
    );

    void open(execution::ExecutionContext& context) override;
    std::optional<execution::ExecutionBatch> next() override;
    void close() override;

    /**
     * @brief Get the accumulated join mask
     */
    execution::EncryptedMask getAccumulatedMask() const;

private:
    crypto::OpenFHECiphertext replicateLeftColumn(
        crypto::OpenFHECiphertext const& col,
        size_t n,
        size_t m,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx
    );

    crypto::OpenFHECiphertext replicateRightColumn(
        crypto::OpenFHECiphertext const& col,
        size_t n,
        size_t m,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx
    );

    std::unique_ptr<Operator> left_child_;
    std::unique_ptr<Operator> right_child_;
    std::string left_table_name_;
    std::string right_table_name_;
    planner::ExprPtr join_condition_;
    execution::ExecutionContext* context_ = nullptr;
    std::unique_ptr<execution::EncryptedMask> accumulated_mask_;
    bool executed_ = false;
};

} // namespace hegadb::server::operators

#endif // JOIN_OPERATOR_H
