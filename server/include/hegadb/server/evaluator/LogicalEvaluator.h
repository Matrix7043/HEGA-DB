#ifndef LOGICAL_EVALUATOR_H
#define LOGICAL_EVALUATOR_H

#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/EncryptedMask.h>

namespace hegadb::server::evaluator {

class LogicalEvaluator {
public:
    explicit LogicalEvaluator(crypto::OpenFHEContext& crypto_context);

    execution::EncryptedMask evaluateAND(
        execution::EncryptedMask const& left,
        execution::EncryptedMask const& right
    );

    execution::EncryptedMask evaluateOR(
        execution::EncryptedMask const& left,
        execution::EncryptedMask const& right
    );

    execution::EncryptedMask evaluateNOT(
        execution::EncryptedMask const& mask
    );

private:
    crypto::OpenFHEContext& crypto_context_;
};

} // namespace hegadb::server::evaluator

#endif // LOGICAL_EVALUATOR_H

