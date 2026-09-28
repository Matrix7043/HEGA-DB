#include <hegadb/server/evaluator/LogicalEvaluator.h>

namespace hegadb::server::evaluator {

LogicalEvaluator::LogicalEvaluator(crypto::OpenFHEContext& crypto_context)
    : crypto_context_(crypto_context) {
}

execution::EncryptedMask LogicalEvaluator::evaluateAND(
    execution::EncryptedMask const& left,
    execution::EncryptedMask const& right) {

    auto const& left_ct = left.getCiphertext();
    auto const& right_ct = right.getCiphertext();

    auto ctx = (left_ct.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // AND: multiply the masks (assuming 0/1 encoding)
    auto result = ctx->EvalMult(left_ct.getCiphertext(), right_ct.getCiphertext());

    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        result,
        left_ct.getSchemeType(),
        left_ct.getOriginalType()
    ));
}

execution::EncryptedMask LogicalEvaluator::evaluateOR(
    execution::EncryptedMask const& left,
    execution::EncryptedMask const& right) {

    auto const& left_ct = left.getCiphertext();
    auto const& right_ct = right.getCiphertext();

    auto ctx = (left_ct.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // OR: left + right - (left * right)
    auto sum = ctx->EvalAdd(left_ct.getCiphertext(), right_ct.getCiphertext());
    auto product = ctx->EvalMult(left_ct.getCiphertext(), right_ct.getCiphertext());
    auto result = ctx->EvalSub(sum, product);

    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        result,
        left_ct.getSchemeType(),
        left_ct.getOriginalType()
    ));
}

execution::EncryptedMask LogicalEvaluator::evaluateNOT(
    execution::EncryptedMask const& mask) {

    auto const& ct = mask.getCiphertext();

    auto ctx = (ct.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // NOT: 1 - mask
    // Negate the mask and add 1 using plaintext operations
    auto result = ctx->EvalNegate(ct.getCiphertext());
    
    // Add constant 1 to the negated mask
    std::vector<int64_t> ones(ctx->GetRingDimension(), 1);
    auto one_plaintext = ctx->MakePackedPlaintext(ones);
    result = ctx->EvalAdd(result, one_plaintext);

    return execution::EncryptedMask(crypto::OpenFHECiphertext(
        result,
        ct.getSchemeType(),
        ct.getOriginalType()
    ));
}

} // namespace hegadb::server::evaluator

