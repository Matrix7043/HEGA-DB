#include <hegadb/server/evaluator/ComparisonEvaluator.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <gtest/gtest.h>

using namespace hegadb::server::evaluator;
using namespace hegadb::server::planner;
using namespace hegadb::crypto;
using namespace hegadb::types;

class ComparisonEvaluatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        context_ = std::make_unique<OpenFHEContext>();
        context_->enableFeatures();

        bfv_ctx_ = context_->getBFVContext();

        auto keypair = bfv_ctx_->KeyGen();
        public_key_ = keypair.publicKey;
        secret_key_ = keypair.secretKey;

        evaluator_ = std::make_unique<ComparisonEvaluator>(*context_);
    }

    OpenFHECiphertext encryptInt(int64_t value) {
        std::vector<int64_t> vec = {value};
        auto plaintext = bfv_ctx_->MakePackedPlaintext(vec);
        auto ct = bfv_ctx_->Encrypt(public_key_, plaintext);
        return OpenFHECiphertext(ct, SchemeType::BFV, DataType::Integer);
    }

    std::unique_ptr<OpenFHEContext> context_;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> bfv_ctx_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> public_key_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> secret_key_;
    std::unique_ptr<ComparisonEvaluator> evaluator_;
};

TEST_F(ComparisonEvaluatorTest, EvaluateEqualityComparison) {
    auto left = encryptInt(42);
    auto right = encryptInt(42);

    auto mask = evaluator_->evaluate(left, ComparisonOp::EQ, right);

    // Mask should be encrypted
    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, EvaluateInequalityComparison) {
    auto left = encryptInt(42);
    auto right = encryptInt(10);

    auto mask = evaluator_->evaluate(left, ComparisonOp::NEQ, right);

    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThan) {
    auto left = encryptInt(10);
    auto right = encryptInt(42);

    auto mask = evaluator_->evaluate(left, ComparisonOp::LT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThan) {
    auto left = encryptInt(42);
    auto right = encryptInt(10);

    auto mask = evaluator_->evaluate(left, ComparisonOp::GT, right);

    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, EvaluateLessThanOrEqual) {
    auto left = encryptInt(42);
    auto right = encryptInt(42);

    auto mask = evaluator_->evaluate(left, ComparisonOp::LTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, EvaluateGreaterThanOrEqual) {
    auto left = encryptInt(42);
    auto right = encryptInt(42);

    auto mask = evaluator_->evaluate(left, ComparisonOp::GTE, right);

    EXPECT_NO_THROW(mask.getCiphertext());
}

TEST_F(ComparisonEvaluatorTest, MaskRemainsEncrypted) {
    auto left = encryptInt(42);
    auto right = encryptInt(10);

    auto mask = evaluator_->evaluate(left, ComparisonOp::EQ, right);

    // The mask should be serializable (indicating it's a valid ciphertext)
    EXPECT_NO_THROW({
        auto serialized = mask.serialize();
        EXPECT_GT(serialized.size(), 0);
    });
}
