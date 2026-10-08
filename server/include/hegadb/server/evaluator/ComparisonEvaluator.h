#ifndef COMPARISON_EVALUATOR_H
#define COMPARISON_EVALUATOR_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/planner/ExpressionPlanner.h>

namespace hegadb::server::evaluator {

class ComparisonEvaluator {
public:
    explicit ComparisonEvaluator(crypto::OpenFHEContext& crypto_context);

    execution::EncryptedMask evaluate(
        crypto::OpenFHECiphertext const& left,
        operations::ComparisonOp op,
        crypto::OpenFHECiphertext const& right);

private:
    execution::EncryptedMask evaluateEQ(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    execution::EncryptedMask evaluateNEQ(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    execution::EncryptedMask evaluateLT(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    execution::EncryptedMask evaluateGT(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    execution::EncryptedMask evaluateLTE(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    execution::EncryptedMask evaluateGTE(
        crypto::OpenFHECiphertext const& left,
        crypto::OpenFHECiphertext const& right);

    crypto::OpenFHEContext& crypto_context_;
};

} // namespace hegadb::server::evaluator

#endif // COMPARISON_EVALUATOR_H
