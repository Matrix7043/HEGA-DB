#include <hegadb/server/execution/ExecutionParameters.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <gtest/gtest.h>

using namespace hegadb::server::execution;
using namespace hegadb::crypto;
using namespace hegadb::types;

class ExecutionParametersTest : public ::testing::Test {
protected:
    void SetUp() override {
        context_ = std::make_unique<OpenFHEContext>();
        context_->enableFeatures();

        bfv_ctx_ = context_->getBFVContext();

        auto keypair = bfv_ctx_->KeyGen();
        public_key_ = keypair.publicKey;
    }

    std::string serializeInt(int64_t value) {
        std::vector<int64_t> vec = {value};
        auto plaintext = bfv_ctx_->MakePackedPlaintext(vec);
        auto ct = bfv_ctx_->Encrypt(public_key_, plaintext);
        OpenFHECiphertext wrapper(ct, SchemeType::BFV, DataType::Integer);
        return wrapper.serialize();
    }

    std::unique_ptr<OpenFHEContext> context_;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> bfv_ctx_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> public_key_;
};

TEST_F(ExecutionParametersTest, DefaultConstructor) {
    ExecutionParameters params;
    EXPECT_EQ(params.size(), 0);
}

TEST_F(ExecutionParametersTest, AddParameter) {
    ExecutionParameters params;

    auto serialized = serializeInt(42);
    params.addParameter(0, serialized);

    EXPECT_TRUE(params.hasParameter(0));
    EXPECT_EQ(params.size(), 1);
}

TEST_F(ExecutionParametersTest, AddMultipleParameters) {
    ExecutionParameters params;

    params.addParameter(0, serializeInt(10));
    params.addParameter(1, serializeInt(20));
    params.addParameter(2, serializeInt(30));

    EXPECT_EQ(params.size(), 3);
    EXPECT_TRUE(params.hasParameter(0));
    EXPECT_TRUE(params.hasParameter(1));
    EXPECT_TRUE(params.hasParameter(2));
}

TEST_F(ExecutionParametersTest, AddNonSequentialParameters) {
    ExecutionParameters params;

    params.addParameter(5, serializeInt(100));

    EXPECT_EQ(params.size(), 6); // Size is 6 (indices 0-5)
    EXPECT_TRUE(params.hasParameter(5));
    EXPECT_FALSE(params.hasParameter(0));
}

TEST_F(ExecutionParametersTest, GetParameter) {
    ExecutionParameters params;

    params.addParameter(0, serializeInt(42));

    auto const& ct = params.getParameter(0);
    EXPECT_EQ(ct.getSchemeType(), SchemeType::BFV);
    EXPECT_EQ(ct.getOriginalType(), DataType::Integer);
}

TEST_F(ExecutionParametersTest, GetParameterThrowsOnMissing) {
    ExecutionParameters params;

    EXPECT_THROW(params.getParameter(0), std::out_of_range);

    params.addParameter(0, serializeInt(42));
    EXPECT_THROW(params.getParameter(1), std::out_of_range);
}

TEST_F(ExecutionParametersTest, HasParameter) {
    ExecutionParameters params;

    EXPECT_FALSE(params.hasParameter(0));

    params.addParameter(0, serializeInt(42));
    EXPECT_TRUE(params.hasParameter(0));
    EXPECT_FALSE(params.hasParameter(1));
}
