#ifndef COMPARISON_EVALUATOR_H
#define COMPARISON_EVALUATOR_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/execution/EncryptedMask.h>
#include <hegadb/server/planner/ExpressionPlanner.h>

#include <vector>

namespace hegadb::server::evaluator {

class ComparisonEvaluator {
public:
    explicit ComparisonEvaluator(crypto::OpenFHEContext& crypto_context);

    /// Compare encrypted column vs encrypted column/parameter
    execution::EncryptedMask evaluate(
        crypto::OpenFHECiphertext const& left,
        operations::ComparisonOp op,
        crypto::OpenFHECiphertext const& right);

    /// Compare encrypted column vs broadcast plaintext parameter
    execution::EncryptedMask evaluate(
        crypto::OpenFHECiphertext const& left,
        operations::ComparisonOp op,
        lbcrypto::Plaintext right_pt);

    /// Compare encrypted column vs scalar literal (packed into broadcast plaintext)
    execution::EncryptedMask evaluate(
        crypto::OpenFHECiphertext const& left,
        operations::ComparisonOp op,
        int64_t literal_val);

private:
    execution::EncryptedMask evaluateOpOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
        operations::ComparisonOp op,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx,
        crypto::SchemeType scheme_type,
        types::DataType original_type);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateEQOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateNEQOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateLTOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateGTOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateLTEOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evaluateGTEOnDiff(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& diff,
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& neg_diff,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    /// Compute base^exp mod p using binary exponentiation by squaring with relinearization
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evalExponentiationBySquaring(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& base,
        uint64_t exponent,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    /// Polynomial evaluation using Paterson-Stockmeyer (Baby-Step Giant-Step)
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> evalPatersonStockmeyer(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> const& x,
        std::vector<int64_t> const& coeffs,
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ctx);

    crypto::OpenFHEContext& crypto_context_;
};

} // namespace hegadb::server::evaluator

#endif // COMPARISON_EVALUATOR_H

