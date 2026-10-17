/*
 * comparison_evaluator_tests.cpp
 *
 * Tests for ComparisonEvaluator with BFV configured for 8-bit signed integers.
 *
 * Context parameters:
 *   p = 65537  (Fermat prime, 2^16+1 — NTT-friendly)
 *   MultiplicativeDepth = 16  (supports diff^65536 = diff^{2^16})
 *   RingDim = 1024, HEStd_NotSet  (fast prototype context)
 *
 * Value constraints (8-bit signed):
 *   All input values must be in [-128, 127].
 *   Differences diff = left - right lie in [-255, 255].
 *   In Z_65537:
 *     positive diff d  =>  slot = d         (in [1, 255])
 *     zero     diff 0  =>  slot = 0
 *     negative diff -k =>  slot = 65537-k   (in [65282, 65536])
 *
 * Test groups:
 *   1. Core functionality  — mixed equal/less/greater slots
 *   2. Boundary values     — values at -128, 0, 127
 *   3. Serialization       — mask remains an opaque ciphertext
 */

#include <gtest/gtest.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/evaluator/ComparisonEvaluator.h>

using namespace hegadb::server::evaluator;
using namespace hegadb::crypto;
using namespace hegadb::types;

// ---------------------------------------------------------------------------
// Fixture — small but valid BFV context for 8-bit signed integer testing
// ---------------------------------------------------------------------------
class ComparisonEvaluatorTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // p=65537: NTT-friendly Fermat prime. (p-1)/2048 = 32 => integer.
        // RingDim=1024, m=2048: (65537-1)/2048 = 32 => NTT root-of-unity valid.
        // Depth=16: supports diff^(p-1) = diff^65536 = diff^{2^{16}}.
        lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS> params;
        params.SetSecurityLevel(lbcrypto::HEStd_NotSet);  // allow small ring for speed
        params.SetRingDim(1024);
        params.SetPlaintextModulus(65537);
        params.SetMultiplicativeDepth(16);

        bfv_ctx_ = lbcrypto::GenCryptoContext(params);
        bfv_ctx_->Enable(lbcrypto::PKE);
        bfv_ctx_->Enable(lbcrypto::KEYSWITCH);
        bfv_ctx_->Enable(lbcrypto::LEVELEDSHE);
        bfv_ctx_->Enable(lbcrypto::ADVANCEDSHE);

        context_ = std::make_unique<OpenFHEContext>();
        context_->setBFVContext(bfv_ctx_);

        auto kp = bfv_ctx_->KeyGen();
        public_key_ = kp.publicKey;
        secret_key_ = kp.secretKey;
        bfv_ctx_->EvalMultKeyGen(secret_key_);

        evaluator_ = std::make_unique<ComparisonEvaluator>(*context_);
    }

    /// Encrypt a single scalar; slot 0 holds the value, rest are 0.
    OpenFHECiphertext encryptScalar(int64_t v)
    {
        auto pt = bfv_ctx_->MakePackedPlaintext(std::vector<int64_t>{ v });
        return OpenFHECiphertext(bfv_ctx_->Encrypt(public_key_, pt),
                                 SchemeType::BFV, DataType::Integer);
    }

    /// Encrypt a small SIMD vector. Values must be in [-128, 127].
    /// Negative values are automatically reduced mod p by OpenFHE.
    OpenFHECiphertext encryptVec(std::vector<int64_t> const& v)
    {
        auto pt = bfv_ctx_->MakePackedPlaintext(v);
        return OpenFHECiphertext(bfv_ctx_->Encrypt(public_key_, pt),
                                 SchemeType::BFV, DataType::Integer);
    }

    /// Decrypt the mask and return the first n slot values as signed integers.
    std::vector<int64_t> decryptSlots(
        hegadb::server::execution::EncryptedMask const& mask, size_t n)
    {
        lbcrypto::Plaintext pt;
        bfv_ctx_->Decrypt(secret_key_,
                          mask.getCiphertext().getCiphertext(), &pt);
        auto raw = pt->GetPackedValue();
        raw.resize(n);
        return raw;
    }

    std::unique_ptr<OpenFHEContext>              context_;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly>  bfv_ctx_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly>       public_key_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly>      secret_key_;
    std::unique_ptr<ComparisonEvaluator>          evaluator_;
};

// ===========================================================================
// Group 1: Core functionality
//
// Operands (8-bit signed, all in [-128, 127]):
//   slot 0: left= 50, right= 50  =>  diff=   0  (equal)
//   slot 1: left= 10, right= 90  =>  diff= -80  (negative: left < right)
//   slot 2: left=100, right= 20  =>  diff=  80  (positive: left > right)
// ===========================================================================

TEST_F(ComparisonEvaluatorTest, EvaluateEqualityComparison)
{
    // EQ: slot0=1, slot1=0, slot2=0
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 1) << "slot0: 50==50 => 1";
    EXPECT_EQ(dec[1], 0) << "slot1: 10==90 => 0";
    EXPECT_EQ(dec[2], 0) << "slot2: 100==20 => 0";
}

TEST_F(ComparisonEvaluatorTest, EvaluateInequalityComparison)
{
    // NEQ: slot0=0, slot1=1, slot2=1
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::NEQ, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 0) << "slot0: 50!=50 => 0";
    EXPECT_EQ(dec[1], 1) << "slot1: 10!=90 => 1";
    EXPECT_EQ(dec[2], 1) << "slot2: 100!=20 => 1";
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThan)
{
    // LT: slot0=0 (50<50 false), slot1=1 (10<90 true), slot2=0 (100<20 false)
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 0) << "slot0: 50<50 => 0";
    EXPECT_EQ(dec[1], 1) << "slot1: 10<90 => 1";
    EXPECT_EQ(dec[2], 0) << "slot2: 100<20 => 0";
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThan)
{
    // GT: slot0=0, slot1=0 (10>90 false), slot2=1 (100>20 true)
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 0) << "slot0: 50>50 => 0";
    EXPECT_EQ(dec[1], 0) << "slot1: 10>90 => 0";
    EXPECT_EQ(dec[2], 1) << "slot2: 100>20 => 1";
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThanOrEqual)
{
    // LE = LT + EQ: slot0=1, slot1=1, slot2=0
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 1) << "slot0: 50<=50 => 1";
    EXPECT_EQ(dec[1], 1) << "slot1: 10<=90 => 1";
    EXPECT_EQ(dec[2], 0) << "slot2: 100<=20 => 0";
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThanOrEqual)
{
    // GE = GT + EQ: slot0=1, slot1=0, slot2=1
    auto left  = encryptVec({  50, 10, 100 });
    auto right = encryptVec({  50, 90,  20 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 1) << "slot0: 50>=50 => 1";
    EXPECT_EQ(dec[1], 0) << "slot1: 10>=90 => 0";
    EXPECT_EQ(dec[2], 1) << "slot2: 100>=20 => 1";
}

TEST_F(ComparisonEvaluatorTest, EvaluateColumnToLiteralComparison)
{
    // EQ vs scalar literal 50: slot0=1, slot1=0, slot2=0
    auto left = encryptVec({ 50, 10, 100 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ,
                                     static_cast<int64_t>(50));

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptSlots(mask, 3);
    EXPECT_EQ(dec[0], 1) << "slot0: 50==50 => 1";
    EXPECT_EQ(dec[1], 0) << "slot1: 10==50 => 0";
    EXPECT_EQ(dec[2], 0) << "slot2: 100==50 => 0";
}

// ===========================================================================
// Group 2: 8-bit signed boundary values
//
// Tests at and near the extreme ends of [-128, 127] verify the indicator
// summation covers the full [-255, 255] difference range.
//
// Boundary operands:
//   slot 0: left=-128, right=-128  => diff=    0  (equal)
//   slot 1: left=-128, right= 127  => diff= -255  (most negative diff)
//   slot 2: left= 127, right=-128  => diff=  255  (most positive diff)
//   slot 3: left=   0, right=   1  => diff=   -1  (smallest negative diff)
//   slot 4: left=   1, right=   0  => diff=    1  (smallest positive diff)
// ===========================================================================

TEST_F(ComparisonEvaluatorTest, BoundaryLessThan)
{
    // LT: [0, 1, 0, 1, 0]
    auto left  = encryptVec({ -128, -128,  127,  0,  1 });
    auto right = encryptVec({ -128,  127, -128,  1,  0 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LT, right);

    auto dec = decryptSlots(mask, 5);
    EXPECT_EQ(dec[0], 0) << "slot0: -128 < -128 => 0";
    EXPECT_EQ(dec[1], 1) << "slot1: -128 <  127 => 1  (max negative diff)";
    EXPECT_EQ(dec[2], 0) << "slot2:  127 < -128 => 0  (max positive diff)";
    EXPECT_EQ(dec[3], 1) << "slot3:    0 <    1 => 1  (diff=-1)";
    EXPECT_EQ(dec[4], 0) << "slot4:    1 <    0 => 0  (diff=+1)";
}

TEST_F(ComparisonEvaluatorTest, BoundaryGreaterThan)
{
    // GT: [0, 0, 1, 0, 1]
    auto left  = encryptVec({ -128, -128,  127,  0,  1 });
    auto right = encryptVec({ -128,  127, -128,  1,  0 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GT, right);

    auto dec = decryptSlots(mask, 5);
    EXPECT_EQ(dec[0], 0) << "slot0: -128 > -128 => 0";
    EXPECT_EQ(dec[1], 0) << "slot1: -128 >  127 => 0";
    EXPECT_EQ(dec[2], 1) << "slot2:  127 > -128 => 1  (max positive diff)";
    EXPECT_EQ(dec[3], 0) << "slot3:    0 >    1 => 0";
    EXPECT_EQ(dec[4], 1) << "slot4:    1 >    0 => 1  (diff=+1)";
}

TEST_F(ComparisonEvaluatorTest, BoundaryEqualityAtExtremes)
{
    // EQ at min and max 8-bit values: both slots should be 1 (equal)
    auto left  = encryptVec({ -128, 127 });
    auto right = encryptVec({ -128, 127 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    auto dec = decryptSlots(mask, 2);
    EXPECT_EQ(dec[0], 1) << "slot0: -128 == -128 => 1";
    EXPECT_EQ(dec[1], 1) << "slot1:  127 ==  127 => 1";
}

TEST_F(ComparisonEvaluatorTest, BoundaryNegativeVsZero)
{
    // Compare negative values against zero
    // slot0: -1 < 0  => LT=1, GT=0, EQ=0
    // slot1:  0 < 0  => LT=0, GT=0, EQ=1
    // slot2:  1 < 0  => LT=0, GT=1, EQ=0
    auto left  = encryptVec({ -1, 0, 1 });
    auto right = encryptVec({  0, 0, 0 });

    auto lt_mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LT, right);
    auto gt_mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GT, right);
    auto eq_mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    auto lt = decryptSlots(lt_mask, 3);
    auto gt = decryptSlots(gt_mask, 3);
    auto eq = decryptSlots(eq_mask, 3);

    EXPECT_EQ(lt[0], 1) << "slot0: -1 <  0 => 1";
    EXPECT_EQ(lt[1], 0) << "slot1:  0 <  0 => 0";
    EXPECT_EQ(lt[2], 0) << "slot2:  1 <  0 => 0";

    EXPECT_EQ(gt[0], 0) << "slot0: -1 >  0 => 0";
    EXPECT_EQ(gt[1], 0) << "slot1:  0 >  0 => 0";
    EXPECT_EQ(gt[2], 1) << "slot2:  1 >  0 => 1";

    EXPECT_EQ(eq[0], 0) << "slot0: -1 == 0 => 0";
    EXPECT_EQ(eq[1], 1) << "slot1:  0 == 0 => 1";
    EXPECT_EQ(eq[2], 0) << "slot2:  1 == 0 => 0";
}

// ===========================================================================
// Group 3: Serialization
// ===========================================================================

TEST_F(ComparisonEvaluatorTest, MaskRemainsEncrypted)
{
    // The output mask must be a valid non-empty serializable ciphertext.
    auto left  = encryptScalar(50);
    auto right = encryptScalar(30);

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    EXPECT_NO_THROW({
        auto serialized = mask.serialize();
        EXPECT_GT(serialized.size(), 0u);
    });
}
