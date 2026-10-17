#include <hegadb/server/execution/ExecutionBatch.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <gtest/gtest.h>

using namespace hegadb::server::execution;
using namespace hegadb::crypto;
using namespace hegadb::types;

class ExecutionBatchTest : public ::testing::Test {
protected:
    void SetUp() override {
        context_ = std::make_unique<OpenFHEContext>();
        context_->enableFeatures();

        bfv_ctx_ = context_->getBFVContext();

        // Generate a keypair for testing
        auto keypair = bfv_ctx_->KeyGen();
        public_key_ = keypair.publicKey;
        secret_key_ = keypair.secretKey;
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
};

TEST_F(ExecutionBatchTest, DefaultConstructor) {
    ExecutionBatch batch;

    EXPECT_EQ(batch.getRowCount(), 0);
    EXPECT_EQ(batch.getColumnCount(), 0);
    EXPECT_TRUE(batch.empty());
}

TEST_F(ExecutionBatchTest, SetAndGetRowCount) {
    ExecutionBatch batch;
    batch.setRowCount(100);

    EXPECT_EQ(batch.getRowCount(), 100);
}

TEST_F(ExecutionBatchTest, AddAndGetColumn) {
    ExecutionBatch batch;
    batch.setRowCount(5);

    auto ct_id = encryptInt(42);
    batch.addColumn("id", std::move(ct_id));

    EXPECT_TRUE(batch.hasColumn("id"));
    EXPECT_FALSE(batch.hasColumn("name"));
    EXPECT_EQ(batch.getColumnCount(), 1);
}

TEST_F(ExecutionBatchTest, GetColumnThrowsOnMissing) {
    ExecutionBatch batch;

    EXPECT_THROW(batch.getColumn("nonexistent"), std::out_of_range);
}

TEST_F(ExecutionBatchTest, AddDuplicateColumnThrows) {
    ExecutionBatch batch;

    auto ct1 = encryptInt(1);
    auto ct2 = encryptInt(2);

    batch.addColumn("id", std::move(ct1));
    EXPECT_THROW(batch.addColumn("id", std::move(ct2)), std::runtime_error);
}

TEST_F(ExecutionBatchTest, GetColumnNames) {
    ExecutionBatch batch;
    batch.setRowCount(3);

    batch.addColumn("id", encryptInt(1));
    batch.addColumn("age", encryptInt(25));
    batch.addColumn("salary", encryptInt(50000));

    auto names = batch.getColumnNames();

    EXPECT_EQ(names.size(), 3);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "id") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "age") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "salary") != names.end());
}

TEST_F(ExecutionBatchTest, RemoveColumn) {
    ExecutionBatch batch;
    batch.addColumn("id", encryptInt(1));
    batch.addColumn("age", encryptInt(25));

    EXPECT_TRUE(batch.removeColumn("id"));
    EXPECT_FALSE(batch.hasColumn("id"));
    EXPECT_TRUE(batch.hasColumn("age"));
    EXPECT_EQ(batch.getColumnCount(), 1);

    EXPECT_FALSE(batch.removeColumn("nonexistent"));
}

TEST_F(ExecutionBatchTest, Clear) {
    ExecutionBatch batch;
    batch.setRowCount(10);
    batch.addColumn("id", encryptInt(1));
    batch.addColumn("age", encryptInt(25));

    batch.clear();

    EXPECT_EQ(batch.getRowCount(), 0);
    EXPECT_EQ(batch.getColumnCount(), 0);
    EXPECT_TRUE(batch.empty());
}

TEST_F(ExecutionBatchTest, EmptyBatch) {
    ExecutionBatch batch;
    EXPECT_TRUE(batch.empty());

    batch.setRowCount(5);
    EXPECT_TRUE(batch.empty()); // Still empty, no columns

    batch.addColumn("id", encryptInt(1));
    EXPECT_FALSE(batch.empty()); // Now has data
}

TEST_F(ExecutionBatchTest, MoveSemantics) {
    ExecutionBatch batch1;
    batch1.setRowCount(5);
    batch1.addColumn("id", encryptInt(42));

    ExecutionBatch batch2(std::move(batch1));

    EXPECT_EQ(batch2.getRowCount(), 5);
    EXPECT_TRUE(batch2.hasColumn("id"));
}
