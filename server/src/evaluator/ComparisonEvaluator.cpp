#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <hegadb/server/evaluator/ComparisonPolynomial.h>
#include <hegadb/logging/Logger.h>
#include <stdexcept>
#include <vector>

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
    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    logging::debug("ComparisonEvaluator: Evaluating CT vs CT comparison");

    auto diff = ctx->EvalSub(left.getCiphertext(), right.getCiphertext());
    auto neg_diff = ctx->EvalSub(right.getCiphertext(), left.getCiphertext());

    return evaluateOpOnDiff(
        diff,
        neg_diff,
        op,
        ctx,
        left.getSchemeType(),
        left.getOriginalType());
}

execution::EncryptedMask ComparisonEvaluator::evaluate(
    crypto::OpenFHECiphertext const& left,
    operations::ComparisonOp op,
    lbcrypto::Plaintext right_pt)
{
    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    logging::debug("ComparisonEvaluator: Evaluating CT vs PT comparison (zero depth sub)");

    auto pt = right_pt;
    auto diff = ctx->EvalSub(left.getCiphertext(), pt);

    // Compute neg_diff = right_pt - left = -(left - right_pt)
    auto neg_diff = ctx->EvalNegate(diff);

    return evaluateOpOnDiff(
        diff,
        neg_diff,
        op,
        ctx,
        left.getSchemeType(),
        left.getOriginalType());
}

execution::EncryptedMask ComparisonEvaluator::evaluate(
    crypto::OpenFHECiphertext const& left,
    operations::ComparisonOp op,
    int64_t literal_val)
{
    auto ctx = (left.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    std::vector<int64_t> broadcast_vec(ctx->GetRingDimension(), literal_val);
    auto pt = ctx->MakePackedPlaintext(broadcast_vec);

    return evaluate(left, op, pt);
}

execution::EncryptedMask ComparisonEvaluator::evaluateOpOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
    operations::ComparisonOp op,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx,
    crypto::SchemeType scheme_type,
    types::DataType original_type)
{
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> result_ct;

    switch (op) {
    case operations::ComparisonOp::EQ:
        result_ct = evaluateEQOnDiff(diff, ctx);
        break;
    case operations::ComparisonOp::NEQ:
        result_ct = evaluateNEQOnDiff(diff, ctx);
        break;
    case operations::ComparisonOp::LT:
        result_ct = evaluateLTOnDiff(diff, ctx);
        break;
    case operations::ComparisonOp::GT:
        result_ct = evaluateGTOnDiff(neg_diff, ctx);
        break;
    case operations::ComparisonOp::LTE:
        result_ct = evaluateLTEOnDiff(diff, neg_diff, ctx);
        break;
    case operations::ComparisonOp::GTE:
        result_ct = evaluateGTEOnDiff(diff, neg_diff, ctx);
        break;
    default:
        logging::error("ComparisonEvaluator: Unknown comparison operator");
        throw std::runtime_error("Unknown comparison operator");
    }

    return execution::EncryptedMask(
        crypto::OpenFHECiphertext(result_ct, scheme_type, original_type),
        execution::MaskKind::DIFF_NONZERO);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evalExponentiationBySquaring(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& base,
    uint64_t exponent,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    if (exponent == 0) {
        // Return ciphertext encrypting 1 in all slots: Enc(0) + 1
        auto zero_ct = ctx->EvalSub(base, base);
        std::vector<int64_t> ones(ctx->GetRingDimension(), 1);
        auto pt_one = ctx->MakePackedPlaintext(ones);
        return ctx->EvalAdd(zero_ct, pt_one);
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> result = nullptr;
    auto power = base;
    uint64_t exp = exponent;

    while (exp > 0) {
        if (exp & 1) {
            if (!result) {
                result = power;
            } else {
                result = ctx->EvalMult(result, power);
                ctx->RelinearizeInPlace(result);
            }
        }
        exp >>= 1;
        if (exp > 0) {
            power = ctx->EvalMult(power, power);
            ctx->RelinearizeInPlace(power);
        }
    }

    return result;
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateEQOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    uint64_t p = ctx->GetCryptoParameters()->GetPlaintextModulus();

    // EQ(x, y) = 1 - (x - y)^(p - 1) mod p
    auto pow_ct = evalExponentiationBySquaring(diff, p - 1, ctx);
    auto neg_pow = ctx->EvalNegate(pow_ct);

    std::vector<int64_t> ones(ctx->GetRingDimension(), 1);
    auto pt_one = ctx->MakePackedPlaintext(ones);

    return ctx->EvalAdd(neg_pow, pt_one);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateNEQOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    uint64_t p = ctx->GetCryptoParameters()->GetPlaintextModulus();

    // NEQ(x, y) = (x - y)^(p - 1) mod p
    return evalExponentiationBySquaring(diff, p - 1, ctx);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateLTOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // Evaluates indicator polynomial P(diff) using Paterson-Stockmeyer.
    // For 8-bit signed differences in [-255, 255] (with p = 65537):
    //   P(diff) = 1 if diff < 0
    //   P(diff) = 0 if diff >= 0
    auto const& coeffs = getLessThanPolynomialCoeffs();
    return evalPatersonStockmeyer(diff, coeffs, ctx);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateGTOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // GT(x, y) = LT(y, x) evaluated on neg_diff = (y - x)
    return evaluateLTOnDiff(neg_diff, ctx);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateLTEOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& /*neg_diff*/,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // LE(x, y) = LT(x, y) + EQ(x, y) mod p
    auto lt_ct = evaluateLTOnDiff(diff, ctx);
    auto eq_ct = evaluateEQOnDiff(diff, ctx);

    return ctx->EvalAdd(lt_ct, eq_ct);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evaluateGTEOnDiff(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    // GE(x, y) = GT(x, y) + EQ(x, y) mod p
    auto gt_ct = evaluateGTOnDiff(neg_diff, ctx);
    auto eq_ct = evaluateEQOnDiff(diff, ctx);

    return ctx->EvalAdd(gt_ct, eq_ct);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ComparisonEvaluator::evalPatersonStockmeyer(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& x,
    std::vector<int64_t> const& coeffs,
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx)
{
    if (coeffs.empty()) {
        return ctx->EvalSub(x, x);
    }

    size_t d = coeffs.size() - 1; // degree
    if (d == 0) {
        std::vector<int64_t> vec_c(ctx->GetRingDimension(), coeffs[0]);
        auto pt_c = ctx->MakePackedPlaintext(vec_c);
        return ctx->EvalAdd(ctx->EvalSub(x, x), pt_c);
    }

    // Determine baby-step parameter k as power of 2 for minimal tree depth
    size_t k = 1;
    while (k * k < d) {
        k *= 2;
    }
    if (k * k > 2 * d && k > 2) {
        k /= 2;
    }
    k = std::max<size_t>(1, std::min(k, d));
    size_t m = (d + k) / k;

    // 1. Powers x^1, ..., x^k computed in a balanced binary tree to minimize multiplicative depth
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> baby_steps(k + 1);
    baby_steps[1] = x;
    if (k >= 2) {
        baby_steps[2] = ctx->EvalMult(x, x);
        ctx->RelinearizeInPlace(baby_steps[2]);
    }
    for (size_t i = 3; i <= k; ++i) {
        size_t half = i / 2;
        baby_steps[i] = ctx->EvalMult(baby_steps[half], baby_steps[i - half]);
        ctx->RelinearizeInPlace(baby_steps[i]);
    }

    // 2. Giant steps w_j = x^(j*k) computed in a balanced binary tree
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> giant_steps(m);
    giant_steps[1] = baby_steps[k];
    for (size_t j = 2; j < m; ++j) {
        size_t half = j / 2;
        giant_steps[j] = ctx->EvalMult(giant_steps[half], giant_steps[j - half]);
        ctx->RelinearizeInPlace(giant_steps[j]);
    }

    // 3. Compute B_j(x) and total polynomial sum
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> total_res = nullptr;

    for (size_t j = 0; j < m; ++j) {
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> block_sum = nullptr;

        for (size_t i = 0; i < k && (j * k + i) <= d; ++i) {
            int64_t coeff = coeffs[j * k + i];
            if (coeff == 0) continue;

            std::vector<int64_t> vec_c(ctx->GetRingDimension(), coeff);
            auto pt_c = ctx->MakePackedPlaintext(vec_c);

            lbcrypto::Ciphertext<lbcrypto::DCRTPoly> term;
            if (i == 0) {
                term = ctx->EvalAdd(ctx->EvalSub(x, x), pt_c);
            } else {
                term = ctx->EvalMult(baby_steps[i], pt_c);
            }

            if (!block_sum) {
                block_sum = term;
            } else {
                block_sum = ctx->EvalAdd(block_sum, term);
            }
        }

        if (block_sum) {
            lbcrypto::Ciphertext<lbcrypto::DCRTPoly> block_term;
            if (j == 0) {
                block_term = block_sum;
            } else {
                block_term = ctx->EvalMult(block_sum, giant_steps[j]);
                ctx->RelinearizeInPlace(block_term);
            }

            if (!total_res) {
                total_res = block_term;
            } else {
                total_res = ctx->EvalAdd(total_res, block_term);
            }
        }
    }

    return total_res ? total_res : ctx->EvalSub(x, x);
}

} // namespace hegadb::server::evaluator

