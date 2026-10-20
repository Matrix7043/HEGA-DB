#include <gtest/gtest.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/Plaintext.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/SegmentReader.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/TableManifest.h>
#include <hegadb/transport/grpc/GrpcClient.h>
#include <hegadb/transport/grpc/GrpcServer.h>
#include <openfhe.h>

#include "common.pb.h"
#include "query.pb.h"
#include "response.pb.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace {

std::vector<int64_t> decryptRawSlots(
    hegadb::crypto::CompositeEncryption const& encryption,
    hegadb::crypto::OpenFHECiphertext const& ct)
{
    auto bfv_ctx = encryption.getContext().getBFVContext();
    auto priv_key = encryption.getKeyManager().getBFVPrivateKey();

    lbcrypto::Plaintext pt;
    bfv_ctx->Decrypt(priv_key, ct.getCiphertext(), &pt);

    int64_t p = static_cast<int64_t>(bfv_ctx->GetCryptoParameters()->GetPlaintextModulus());
    int64_t half_p = p / 2;

    std::vector<int64_t> raw = pt->GetPackedValue();
    std::vector<int64_t> signed_values;
    signed_values.reserve(raw.size());

    for (int64_t val : raw) {
        signed_values.push_back((val > half_p) ? (val - p) : val);
    }
    return signed_values;
}

class ServerCreateAndInsertTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        test_root_ = "/tmp/hegadb_server_test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        data_dir_ = test_root_ + "/data";
        catalog_dir_ = test_root_ + "/catalog";
        key_dir_ = test_root_ + "/keys";

        std::filesystem::create_directories(data_dir_);
        std::filesystem::create_directories(catalog_dir_);
        std::filesystem::create_directories(key_dir_);

        encryption_ = std::make_unique<hegadb::crypto::CompositeEncryption>(key_dir_);

        // Generate rotation keys for powers/steps
        auto bfv_ctx = encryption_->getContext().getBFVContext();
        auto priv_key = encryption_->getKeyManager().getBFVPrivateKey();
        std::vector<int32_t> rot_indices = {1, 2, 3, 4, 5, 6, 7, 8, -1, -2, -3, -4, -5, -6, -7, -8};
        bfv_ctx->EvalRotateKeyGen(priv_key, rot_indices);

        crypto_context_ = std::make_shared<hegadb::crypto::OpenFHEContext>();
        crypto_context_->enableFeatures();

        std::unordered_map<std::string, hegadb::catalog::TableSchema> tables;
        auto snapshot = std::make_shared<hegadb::catalog::CatalogSnapshot const>(1, std::move(tables));
        catalog_ = std::make_shared<hegadb::catalog::InMemoryCatalog>(snapshot);

        storage_ = hegadb::server::storage::StorageManager::createFileStorage(data_dir_, catalog_dir_);

        executor_ = std::make_shared<hegadb::server::QueryExecutor>(
            catalog_, storage_, crypto_context_, nullptr);

        receiver_ = std::make_shared<hegadb::server::ExecuteQueryReceiver>(
            catalog_, executor_);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(test_root_);
    }

    std::string test_root_;
    std::string data_dir_;
    std::string catalog_dir_;
    std::string key_dir_;

    std::unique_ptr<hegadb::crypto::CompositeEncryption> encryption_;
    std::shared_ptr<hegadb::crypto::OpenFHEContext> crypto_context_;
    std::shared_ptr<hegadb::catalog::InMemoryCatalog> catalog_;
    std::shared_ptr<hegadb::server::storage::StorageManager> storage_;
    std::shared_ptr<hegadb::server::QueryExecutor> executor_;
    std::shared_ptr<hegadb::server::ExecuteQueryReceiver> receiver_;
};

TEST_F(ServerCreateAndInsertTest, CreateTableGeneratesFilesAndManifest)
{
    hegadb::protocol::ExecuteQuery request;
    request.mutable_protocol_version()->set_major(1);
    request.mutable_protocol_version()->set_minor(1);
    request.mutable_request_id()->set_value("req-create-001");

    auto* create = request.mutable_statement()->mutable_create_table_statement();
    create->set_table("CUSTOMERS");

    auto* col1 = create->add_columns();
    col1->set_name("id");
    col1->set_data_type(hegadb::protocol::DATA_TYPE_INTEGER);
    col1->set_nullable(false);

    auto* col2 = create->add_columns();
    col2->set_name("active");
    col2->set_data_type(hegadb::protocol::DATA_TYPE_BOOLEAN);
    col2->set_nullable(false);

    auto response = receiver_->executeQuery(request);

    EXPECT_EQ(response.status().code(), hegadb::protocol::STATUS_OK);
    EXPECT_NE(response.status().message().find("CUSTOMERS"), std::string::npos);

    // Verify directory exists
    EXPECT_TRUE(std::filesystem::exists(data_dir_ + "/CUSTOMERS"));
    EXPECT_TRUE(std::filesystem::exists(data_dir_ + "/CUSTOMERS/id.seg"));
    EXPECT_TRUE(std::filesystem::exists(data_dir_ + "/CUSTOMERS/active.seg"));

    // Verify manifest
    auto manifest_path = catalog_dir_ + "/CUSTOMERS.manifest.json";
    EXPECT_TRUE(std::filesystem::exists(manifest_path));

    auto manifest = hegadb::server::storage::TableManifest::deserialize(manifest_path);
    ASSERT_TRUE(manifest.has_value());
    EXPECT_EQ(manifest->table_name, "CUSTOMERS");
    EXPECT_EQ(manifest->total_records, 0);
    EXPECT_EQ(manifest->columns.size(), 2);
    EXPECT_EQ(manifest->columns[0].name, "id");
    EXPECT_EQ(manifest->columns[0].type, "INTEGER");
    EXPECT_EQ(manifest->columns[1].name, "active");
    EXPECT_EQ(manifest->columns[1].type, "BOOLEAN");

    // Verify local catalog updated
    auto const* table = executor_->getCatalog()->findTable("CUSTOMERS");
    ASSERT_NE(table, nullptr);
    EXPECT_EQ(table->name(), "CUSTOMERS");
    EXPECT_EQ(table->columns().size(), 2);
}

TEST_F(ServerCreateAndInsertTest, CreateTableDuplicateThrowsError)
{
    hegadb::protocol::ExecuteQuery request;
    request.mutable_protocol_version()->set_major(1);
    request.mutable_protocol_version()->set_minor(1);
    request.mutable_request_id()->set_value("req-create-dup");

    auto* create = request.mutable_statement()->mutable_create_table_statement();
    create->set_table("PRODUCTS");

    auto* col = create->add_columns();
    col->set_name("id");
    col->set_data_type(hegadb::protocol::DATA_TYPE_INTEGER);
    col->set_nullable(false);

    auto response1 = receiver_->executeQuery(request);
    EXPECT_EQ(response1.status().code(), hegadb::protocol::STATUS_OK);

    // Second creation of same table must return STATUS_ERROR
    auto response2 = receiver_->executeQuery(request);
    EXPECT_EQ(response2.status().code(), hegadb::protocol::STATUS_ERROR);
    EXPECT_NE(response2.status().message().find("already exists"), std::string::npos);
}

TEST_F(ServerCreateAndInsertTest, InsertSingleRowSuccess)
{
    // Step 1: Create table
    hegadb::protocol::ExecuteQuery create_req;
    create_req.mutable_protocol_version()->set_major(1);
    create_req.mutable_protocol_version()->set_minor(1);
    create_req.mutable_request_id()->set_value("req-create-insert");

    auto* create = create_req.mutable_statement()->mutable_create_table_statement();
    create->set_table("ITEMS");
    auto* col1 = create->add_columns();
    col1->set_name("id");
    col1->set_data_type(hegadb::protocol::DATA_TYPE_INTEGER);
    col1->set_nullable(false);

    auto* col2 = create->add_columns();
    col2->set_name("active");
    col2->set_data_type(hegadb::protocol::DATA_TYPE_BOOLEAN);
    col2->set_nullable(false);

    auto create_resp = receiver_->executeQuery(create_req);
    ASSERT_EQ(create_resp.status().code(), hegadb::protocol::STATUS_OK);

    // Step 2: Encrypt values with encryptInsert (Slot 0)
    auto id_ct = encryption_->encryptInsert(hegadb::crypto::Plaintext(42LL));
    auto active_ct = encryption_->encryptInsert(hegadb::crypto::Plaintext(true));

    auto* id_openfhe = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(id_ct.get());
    auto* active_openfhe = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(active_ct.get());
    ASSERT_NE(id_openfhe, nullptr);
    ASSERT_NE(active_openfhe, nullptr);

    // Step 3: Send INSERT query
    hegadb::protocol::ExecuteQuery insert_req;
    insert_req.mutable_protocol_version()->set_major(1);
    insert_req.mutable_protocol_version()->set_minor(1);
    insert_req.mutable_request_id()->set_value("req-insert-001");

    auto* insert = insert_req.mutable_statement()->mutable_insert_statement();
    insert->set_table("ITEMS");
    insert->add_columns("id");
    insert->add_columns("active");

    auto* p0 = insert_req.add_parameters();
    p0->set_index(0);
    p0->set_ciphertext(id_openfhe->serialize());

    auto* p1 = insert_req.add_parameters();
    p1->set_index(1);
    p1->set_ciphertext(active_openfhe->serialize());

    auto insert_resp = receiver_->executeQuery(insert_req);
    EXPECT_EQ(insert_resp.status().code(), hegadb::protocol::STATUS_OK);
    EXPECT_NE(insert_resp.status().message().find("1 row inserted"), std::string::npos);

    // Step 4: Verify storage
    auto table = storage_->openTable("ITEMS");
    ASSERT_NE(table, nullptr);
    EXPECT_EQ(table->getRowCount(), 1);

    table->open();
    auto batch = table->nextBatch();
    ASSERT_TRUE(batch.has_value());
    EXPECT_EQ(batch->row_count, 1);

    auto const& id_stored = batch->columns.at("id");
    auto id_slots = decryptRawSlots(*encryption_, id_stored);
    EXPECT_EQ(id_slots[0], 42);
    EXPECT_EQ(id_slots[1], 0);

    auto const& active_stored = batch->columns.at("active");
    auto active_slots = decryptRawSlots(*encryption_, active_stored);
    EXPECT_EQ(active_slots[0], 1);
    EXPECT_EQ(active_slots[1], 0);

    table->close();
}

TEST_F(ServerCreateAndInsertTest, InsertMultipleRowsWithHomomorphicRotation)
{
    // Step 1: Create table
    hegadb::protocol::ExecuteQuery create_req;
    create_req.mutable_protocol_version()->set_major(1);
    create_req.mutable_protocol_version()->set_minor(1);
    create_req.mutable_request_id()->set_value("req-create-multi");

    auto* create = create_req.mutable_statement()->mutable_create_table_statement();
    create->set_table("SCORES");
    auto* col = create->add_columns();
    col->set_name("val");
    col->set_data_type(hegadb::protocol::DATA_TYPE_INTEGER);
    col->set_nullable(false);

    auto create_resp = receiver_->executeQuery(create_req);
    ASSERT_EQ(create_resp.status().code(), hegadb::protocol::STATUS_OK);

    // Step 2: Insert 3 consecutive rows: 10, 20, 30
    std::vector<int64_t> values_to_insert = {10, 20, 30};

    for (size_t i = 0; i < values_to_insert.size(); ++i) {
        auto val_ct = encryption_->encryptInsert(hegadb::crypto::Plaintext(values_to_insert[i]));
        auto* val_openfhe = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(val_ct.get());

        hegadb::protocol::ExecuteQuery insert_req;
        insert_req.mutable_protocol_version()->set_major(1);
        insert_req.mutable_protocol_version()->set_minor(1);
        insert_req.mutable_request_id()->set_value("req-insert-" + std::to_string(i));

        auto* insert = insert_req.mutable_statement()->mutable_insert_statement();
        insert->set_table("SCORES");
        insert->add_columns("val");

        auto* p = insert_req.add_parameters();
        p->set_index(0);
        p->set_ciphertext(val_openfhe->serialize());

        auto resp = receiver_->executeQuery(insert_req);
        ASSERT_EQ(resp.status().code(), hegadb::protocol::STATUS_OK);
    }

    // Step 3: Verify the column ciphertext has values accumulated across slots 0, 1, 2
    auto table = storage_->openTable("SCORES");
    ASSERT_NE(table, nullptr);
    EXPECT_EQ(table->getRowCount(), 3);

    table->open();
    auto batch = table->nextBatch();
    ASSERT_TRUE(batch.has_value());
    EXPECT_EQ(batch->row_count, 3);

    auto const& val_col = batch->columns.at("val");
    auto slots = decryptRawSlots(*encryption_, val_col);

    EXPECT_EQ(slots[0], 10);
    EXPECT_EQ(slots[1], 20);
    EXPECT_EQ(slots[2], 30);
    EXPECT_EQ(slots[3], 0);

    table->close();
}

TEST_F(ServerCreateAndInsertTest, EndToEndGrpcRoundTrip)
{
    // Start GrpcServer
    std::string server_addr = "127.0.0.1:0"; // Ephemeral port
    auto server = std::make_unique<hegadb::transport::grpc::GrpcServer>(server_addr);

    auto receiver = receiver_;
    server->setRequestHandler([receiver](hegadb::protocol::ExecuteQuery const& req) {
        return receiver->executeQuery(req);
    });
    server->start();

    hegadb::transport::grpc::GrpcClient client(server->address());

    // 1. Create table over gRPC
    hegadb::protocol::ExecuteQuery create_req;
    create_req.mutable_protocol_version()->set_major(1);
    create_req.mutable_protocol_version()->set_minor(1);
    create_req.mutable_request_id()->set_value("grpc-create");

    auto* create = create_req.mutable_statement()->mutable_create_table_statement();
    create->set_table("GRPC_TABLE");
    auto* c = create->add_columns();
    c->set_name("num");
    c->set_data_type(hegadb::protocol::DATA_TYPE_INTEGER);
    c->set_nullable(false);

    auto create_resp = client.execute(create_req);
    EXPECT_EQ(create_resp.status().code(), hegadb::protocol::STATUS_OK);
    EXPECT_NE(create_resp.status().message().find("GRPC_TABLE"), std::string::npos);

    // 2. Insert row over gRPC
    auto num_ct = encryption_->encryptInsert(hegadb::crypto::Plaintext(77LL));
    auto* openfhe_ct = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(num_ct.get());

    hegadb::protocol::ExecuteQuery insert_req;
    insert_req.mutable_protocol_version()->set_major(1);
    insert_req.mutable_protocol_version()->set_minor(1);
    insert_req.mutable_request_id()->set_value("grpc-insert");

    auto* insert = insert_req.mutable_statement()->mutable_insert_statement();
    insert->set_table("GRPC_TABLE");
    insert->add_columns("num");

    auto* p = insert_req.add_parameters();
    p->set_index(0);
    p->set_ciphertext(openfhe_ct->serialize());

    auto insert_resp = client.execute(insert_req);
    EXPECT_EQ(insert_resp.status().code(), hegadb::protocol::STATUS_OK);
    EXPECT_NE(insert_resp.status().message().find("1 row inserted"), std::string::npos);

    server->shutdown();
}

} // namespace
