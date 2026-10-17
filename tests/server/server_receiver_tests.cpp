#include "hegadb/transport/grpc/GrpcClient.h"
#include "hegadb/transport/grpc/GrpcServer.h"
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace {

constexpr uint32_t TEST_MAJOR_VERSION = 1;
constexpr uint32_t TEST_MINOR_VERSION = 1;

// Helper to create test dependencies for ExecuteQueryReceiver
std::shared_ptr<hegadb::server::ExecuteQueryReceiver> createTestReceiver()
{
    // Create a catalog snapshot with STUDENTS table
    std::vector<hegadb::catalog::ColumnSchema> columns = {
        {"ID", hegadb::types::DataType::Integer, false, 0},
        {"NAME", hegadb::types::DataType::String, false, 1},
        {"GRADE", hegadb::types::DataType::Integer, false, 2}
    };
    
    hegadb::catalog::TableSchema students_table("STUDENTS", columns);
    
    std::unordered_map<std::string, hegadb::catalog::TableSchema> tables;
    tables.emplace("STUDENTS", std::move(students_table));
    auto snapshot = std::make_shared<hegadb::catalog::CatalogSnapshot const>(1, std::move(tables));

    auto catalog = std::make_shared<hegadb::catalog::InMemoryCatalog>(snapshot);
    auto crypto_context = std::make_shared<hegadb::crypto::OpenFHEContext>();
    crypto_context->enableFeatures();

    // Create a mock storage manager for testing
    class MockStorageManager : public hegadb::server::storage::StorageManager {
    private:
        class MockEncryptedTable : public hegadb::server::storage::EncryptedTable {
        private:
            std::string table_name_;
            std::vector<std::string> column_names_;
            bool is_open_ = false;
            bool batch_returned_ = false;
            std::shared_ptr<hegadb::crypto::OpenFHEContext> crypto_ctx_;
            
        public:
            MockEncryptedTable(std::string name, 
                             std::vector<std::string> columns,
                             std::shared_ptr<hegadb::crypto::OpenFHEContext> crypto_ctx)
                : table_name_(std::move(name))
                , column_names_(std::move(columns))
                , crypto_ctx_(std::move(crypto_ctx))
            {}
            
            std::string const& getName() const override {
                return table_name_;
            }
            
            std::vector<std::string> getColumnNames() const override {
                return column_names_;
            }
            
            void open() override {
                is_open_ = true;
                batch_returned_ = false;
            }
            
            std::optional<hegadb::server::storage::EncryptedBatch> nextBatch() override {
                if (!is_open_ || batch_returned_) {
                    return std::nullopt;
                }
                
                // Return one empty batch (0 rows of encrypted data)
                // This simulates a table with no data that matches the query
                batch_returned_ = true;
                
                hegadb::server::storage::EncryptedBatch batch;
                batch.row_count = 0;
                
                // Don't add any column data since row_count is 0
                
                return batch;
            }
            
            void close() override {
                is_open_ = false;
            }
            
            size_t getRowCount() const override {
                return 0; // Empty table for testing
            }
        };
        
        std::shared_ptr<hegadb::crypto::OpenFHEContext> crypto_ctx_;
        std::shared_ptr<hegadb::catalog::Catalog const> catalog_;
        
    public:
        MockStorageManager(std::shared_ptr<hegadb::crypto::OpenFHEContext> crypto_ctx,
                          std::shared_ptr<hegadb::catalog::Catalog const> catalog)
            : crypto_ctx_(std::move(crypto_ctx))
            , catalog_(std::move(catalog))
        {}
        
        std::unique_ptr<hegadb::server::storage::EncryptedTable> openTable(
            std::string const& table_name) override
        {
            // Look up the table in catalog to get column names
            auto* table_schema = catalog_->findTable(table_name);
            if (!table_schema) {
                throw std::runtime_error("Table not found in catalog: " + table_name);
            }
            
            std::vector<std::string> column_names;
            for (auto const& col : table_schema->columns()) {
                column_names.push_back(col.name);
            }
            
            // Return a mock table that returns one empty batch
            return std::make_unique<MockEncryptedTable>(
                table_name, 
                std::move(column_names),
                crypto_ctx_
            );
        }
    };

    auto storage = std::make_shared<MockStorageManager>(crypto_context, catalog);
    auto executor = std::make_shared<hegadb::server::QueryExecutor>(
        catalog, storage, crypto_context);
    return std::make_shared<hegadb::server::ExecuteQueryReceiver>(catalog, executor);
}

// Helper to construct a basic valid ExecuteQuery proto
hegadb::protocol::ExecuteQuery createValidQuery(std::string const& req_id = "req-001")
{
    hegadb::protocol::ExecuteQuery query;

    // Protocol Version
    query.mutable_protocol_version()->set_major(TEST_MAJOR_VERSION);
    query.mutable_protocol_version()->set_minor(TEST_MINOR_VERSION);

    // Request ID
    query.mutable_request_id()->set_value(req_id);

    // Statement: SELECT * FROM STUDENTS
    auto* select = query.mutable_statement()->mutable_select_statement();
    select->mutable_from_table()->set_table("STUDENTS");
    select->add_projections()->mutable_wildcard();

    return query;
}

} // namespace

// ============================================================================
// Unit Tests for ExecuteQueryReceiver
// ============================================================================

TEST(ExecuteQueryReceiverTest, HandlesValidQuerySuccessfully)
{
    auto receiver = createTestReceiver();
    auto query = createValidQuery("test-valid-01");

    auto response = receiver->executeQuery(query);

    // Assert version & request ID preservation
    EXPECT_EQ(response.protocol_version().major(), TEST_MAJOR_VERSION);
    EXPECT_EQ(response.protocol_version().minor(), TEST_MINOR_VERSION);
    EXPECT_EQ(response.request_id().value(), "test-valid-01");

    // Assert status OK
    ASSERT_TRUE(response.has_status());
    if (response.status().code() != hegadb::protocol::STATUS_OK) {
        std::cerr << "ERROR: " << response.status().message() << std::endl;
    }
    EXPECT_EQ(response.status().code(), hegadb::protocol::STATUS_OK);
}

TEST(ExecuteQueryReceiverTest, RejectsProtocolVersionMismatch)
{
    auto receiver = createTestReceiver();
    auto query = createValidQuery("test-version-mismatch");

    // Incompatible version (9.9)
    query.mutable_protocol_version()->set_major(9);
    query.mutable_protocol_version()->set_minor(9);

    auto response = receiver->executeQuery(query);

    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), hegadb::protocol::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("unsupported"), std::string::npos);
}

// ============================================================================
// End-to-End Integration Test over gRPC
// ============================================================================

class ServerTransportIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        server_ = std::make_unique<hegadb::transport::grpc::GrpcServer>("127.0.0.1:0");
        receiver_ = createTestReceiver();

        // Bind receiver callback
        server_->setRequestHandler(
            [this](hegadb::protocol::ExecuteQuery const& request) {
                return receiver_->executeQuery(request);
            });

        server_->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        address_ = server_->address();
    }

    void TearDown() override
    {
        server_->shutdown();
    }

    std::string address_;
    std::unique_ptr<hegadb::transport::grpc::GrpcServer> server_;
    std::shared_ptr<hegadb::server::ExecuteQueryReceiver> receiver_;
};

TEST_F(ServerTransportIntegrationTest, FullRoundTripOverGrpc)
{
    hegadb::transport::grpc::GrpcClient client(address_);
    auto request = createValidQuery("grpc-integration-001");

    auto response = client.execute(request);

    EXPECT_EQ(response.request_id().value(), "grpc-integration-001");
    EXPECT_EQ(response.status().code(), hegadb::protocol::STATUS_OK);
}
