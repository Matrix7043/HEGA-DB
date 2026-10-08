#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <stdexcept>

namespace hegadb::server::evaluator {

ComparisonEvaluator::ComparisonEvaluator(crypto::OpenFHEContext& crypto_context)
    : crypto_context_(crypto_context)
{
}

execution::EncryptedMask ComparisonEvaluator::evaluate(
    crypto::OpenFHECiphertext const& left,
    operations::ComparisonOp op,
    crypto::OpenFHECiphertext const& right)
{

    switch (op) {
    case operations::ComparisonOp::EQ:
        return evaluateEQ(left, right);
    case operations::ComparisonOp::NEQ:
        return evaluateNEQ(left, right);
    case operations::ComparisonOp::LT:
        return evaluateLT(left, right);
    case operations::ComparisonOp::GT:
        return evaluateGT(left, right);
    case operations::ComparisonOp::LTE:
        return evaluateLTE(left, right);
    case operations::ComparisonOp::GTE:
        return evaluateGTE(left, right);
    default:
        throw std::runtime_error("Unknown comparison operator");
    }
}

execution::EncryptedMask ComparisonEvaluator::evaluateEQ(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot == 0 means left == right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_ZERO);
}

execution::EncryptedMask ComparisonEvaluator::evaluateNEQ(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot != 0 means left != right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_NONZERO);
}

execution::EncryptedMask ComparisonEvaluator::evaluateLT(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot < 0 means left < right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_NEGATIVE);
}

execution::EncryptedMask ComparisonEvaluator::evaluateGT(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot > 0 means left > right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_POSITIVE);
}

execution::EncryptedMask ComparisonEvaluator::evaluateLTE(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot <= 0 means left <= right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_NONPOS);
}

execution::EncryptedMask ComparisonEvaluator::evaluateGTE(
    crypto::OpenFHECiphertext const& left,
    crypto::OpenFHECiphertext const& right)
{

    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Store Enc(left - right): slot >= 0 means left >= right
    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(diff, left.getSchemeType(), left.getOriginalType()),
        execution::MaskKind::DIFF_NONNEG);
}

} // namespace hegadb::server::evaluator
