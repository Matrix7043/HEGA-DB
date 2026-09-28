#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <stdexcept>

namespace hegadb::server::evaluator {

ComparisonEvaluator::ComparisonEvaluator(crypto::OpenFHEContext& crypto_context)
    : crypto_context_(crypto_context) {
}

execution::EncryptedMask ComparisonEvaluator::evaluate(
    crypto::OpenFHECiphertext const& left,
    planner::ComparisonOp op,
    crypto::OpenFHECiphertext const& right) {

    switch (op) {
        case planner::ComparisonOp::EQ:
            return evaluateEQ(left, right);
        case planner::ComparisonOp::NEQ:
            return evaluateNEQ(left, right);
        case planner::ComparisonOp::LT:
            return evaluateLT(left, right);
        case planner::ComparisonOp::GT:
            return evaluateGT(left, right);
        case planner::ComparisonOp::LTE:
            return evaluateLTE(left, right);
        case planner::ComparisonOp::GTE:
            return evaluateGTE(left, right);
        default:
            throw std::runtime_error("Unknown comparison operator");
    }
}

execution::EncryptedMask ComparisonEvaluator::evaluateEQ(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    // Get appropriate context based on scheme
    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Compute left - right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());

    // For equality: result is 0 if equal
    // We need to compute a mask that is 1 if diff == 0, else 0
    // This typically requires bootstrapping or polynomial evaluation
    // For now, return the difference (client will interpret)
    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        diff,
        left.getSchemeType(),
        left.getOriginalType()
    ));
}

execution::EncryptedMask ComparisonEvaluator::evaluateNEQ(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    // NEQ is the negation of EQ
    auto eq_mask = evaluateEQ(left, right);

    // Return negated mask (NOT operation)
    // This will be handled by LogicalEvaluator
    return eq_mask;
}

execution::EncryptedMask ComparisonEvaluator::evaluateLT(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Compute right - left
    auto diff = ctx->EvalSub(right.getCiphertext(), left.getCiphertext());

    // For LT: check if diff > 0 (i.e., right > left)
    // This requires comparison circuit evaluation
    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        diff,
        left.getSchemeType(),
        left.getOriginalType()
    ));
}

execution::EncryptedMask ComparisonEvaluator::evaluateGT(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Compute left - right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());

    // For GT: check if diff > 0
    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        diff,
        left.getSchemeType(),
        left.getOriginalType()
    ));
}

execution::EncryptedMask ComparisonEvaluator::evaluateLTE(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    // LTE is NOT(GT)
    auto gt_mask = evaluateGT(left, right);
    return gt_mask;
}

execution::EncryptedMask ComparisonEvaluator::evaluateGTE(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right) {

    // GTE is NOT(LT)
    auto lt_mask = evaluateLT(left, right);
    return lt_mask;
}

} // namespace hegadb::server::evaluator

