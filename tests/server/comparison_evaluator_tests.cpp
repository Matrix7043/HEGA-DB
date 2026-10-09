#include <gtest/gtest.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/evaluator/ComparisonEvaluator.h>

using namespace hegadb::server::evaluator;
using namespace hegadb::server::planner;
using namespace hegadb::crypto;
using namespace hegadb::types;

class ComparisonEvaluatorTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS> parameters;
        parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);
        parameters.SetPlaintextModulus(65537);
        parameters.SetMultiplicativeDepth(16);
        parameters.SetBatchSize(8192);

        bfv_ctx_ = lbcrypto::GenCryptoContext(parameters);
        bfv_ctx_->Enable(lbcrypto::PKE);
        bfv_ctx_->Enable(lbcrypto::KEYSWITCH);
        bfv_ctx_->Enable(lbcrypto::LEVELEDSHE);
        bfv_ctx_->Enable(lbcrypto::ADVANCEDSHE);

        context_ = std::make_unique<OpenFHEContext>();
        context_->setBFVContext(bfv_ctx_);

        auto keypair = bfv_ctx_->KeyGen();
        public_key_ = keypair.publicKey;
        secret_key_ = keypair.secretKey;

        bfv_ctx_->EvalMultKeyGen(secret_key_);

        evaluator_ = std::make_unique<ComparisonEvaluator>(*context_);
    }

    OpenFHECiphertext encryptInt(int64_t value)
    {
        std::vector<int64_t> vec = { value };
        auto plaintext = bfv_ctx_->MakePackedPlaintext(vec);
        auto ct = bfv_ctx_->Encrypt(public_key_, plaintext);
        return OpenFHECiphertext(ct, SchemeType::BFV, DataType::Integer);
    }

    OpenFHECiphertext encryptIntVec(std::vector<int64_t> const& values)
    {
        auto plaintext = bfv_ctx_->MakePackedPlaintext(values);
        auto ct = bfv_ctx_->Encrypt(public_key_, plaintext);
        return OpenFHECiphertext(ct, SchemeType::BFV, DataType::Integer);
    }

    std::vector<int64_t> decryptMaskVec(hegadb::server::execution::EncryptedMask const& mask, size_t num_slots = 3)
    {
        lbcrypto::Plaintext pt;
        bfv_ctx_->Decrypt(secret_key_, mask.getCiphertext().getCiphertext(), &pt);
        auto vec = pt->GetPackedValue();
        vec.resize(num_slots);
        return vec;
    }

    std::unique_ptr<OpenFHEContext> context_;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> bfv_ctx_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> public_key_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> secret_key_;
    std::unique_ptr<ComparisonEvaluator> evaluator_;
};

TEST_F(ComparisonEvaluatorTest, EvaluateEqualityComparison)
{
    auto left = encryptIntVec({ 42, 10, 5 });
    auto right = encryptIntVec({ 42, 20, 5 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 0);
    EXPECT_EQ(dec[2], 1);
}

TEST_F(ComparisonEvaluatorTest, EvaluateInequalityComparison)
{
    auto left = encryptIntVec({ 42, 10, 5 });
    auto right = encryptIntVec({ 42, 20, 5 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::NEQ, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 0);
    EXPECT_EQ(dec[1], 1);
    EXPECT_EQ(dec[2], 0);
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThan)
{
    auto left = encryptIntVec({ 10, 42, 5 });
    auto right = encryptIntVec({ 20, 42, 3 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 0);
    EXPECT_EQ(dec[2], 0);
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThan)
{
    auto left = encryptIntVec({ 42, 10, 5 });
    auto right = encryptIntVec({ 20, 42, 3 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 0);
    EXPECT_EQ(dec[2], 1);
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThanOrEqual)
{
    auto left = encryptIntVec({ 10, 42, 5 });
    auto right = encryptIntVec({ 20, 42, 5 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::LTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 1);
    EXPECT_EQ(dec[2], 1);
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThanOrEqual)
{
    auto left = encryptIntVec({ 42, 10, 3 });
    auto right = encryptIntVec({ 20, 42, 5 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::GTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 0);
    EXPECT_EQ(dec[2], 0);
}

TEST_F(ComparisonEvaluatorTest, EvaluateColumnToLiteralComparison)
{
    auto left = encryptIntVec({ 42, 10, 42 });

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, 42);

    EXPECT_NO_THROW(mask.getCiphertext());
    auto dec = decryptMaskVec(mask, 3);
    EXPECT_EQ(dec[0], 1);
    EXPECT_EQ(dec[1], 0);
    EXPECT_EQ(dec[2], 1);
}

TEST_F(ComparisonEvaluatorTest, MaskRemainsEncrypted)
{
    auto left = encryptInt(42);
    auto right = encryptInt(10);

    auto mask = evaluator_->evaluate(left, hegadb::operations::ComparisonOp::EQ, right);

    EXPECT_NO_THROW({
        auto serialized = mask.serialize();
        EXPECT_GT(serialized.size(), 0);
    });
}

