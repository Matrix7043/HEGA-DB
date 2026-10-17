#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/DropTableStmt.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/transport/grpc/GrpcClient.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

using hegadb::client::SQLParser;
using hegadb::client::ast::ColumnDef;
using hegadb::client::ast::CreateTableStmt;
using hegadb::client::ast::DropTableStmt;
using hegadb::client::protocol::ExecuteQueryEncoder;
namespace proto = hegadb::protocol;

// Creates a minimal ExecuteQueryReceiver wired to an in-memory catalog
// that contains the tables referenced by the gRPC tests.
std::shared_ptr<hegadb::server::ExecuteQueryReceiver> createTestReceiver()
{
    using hegadb::catalog::ColumnSchema;
    using hegadb::catalog::TableSchema;
    using hegadb::types::DataType;

    std::unordered_map<std::string, TableSchema> tables;

    // STUDENTS — used by DdlGrpcTest.SelectStillSucceeds
    tables.emplace("STUDENTS", TableSchema("STUDENTS", std::vector<ColumnSchema>{
        { "ID",    DataType::Integer, false, 0 },
        { "NAME",  DataType::String,  false, 1 },
        { "GRADE", DataType::Integer, false, 2 },
    }));
    auto snapshot = std::make_shared<hegadb::catalog::CatalogSnapshot const>(
        1, std::move(tables));
    auto catalog = std::make_shared<hegadb::catalog::InMemoryCatalog>(snapshot);

    auto crypto_context = std::make_shared<hegadb::crypto::OpenFHEContext>();
    crypto_context->enableFeatures();

    class MockStorageManager : public hegadb::server::storage::StorageManager {

        class EmptyEncryptedTable : public hegadb::server::storage::EncryptedTable {
            std::string name_;
        public:
            explicit EmptyEncryptedTable(std::string name) : name_(std::move(name)) {}
            std::string const& getName() const override { return name_; }
            std::vector<std::string> getColumnNames() const override { return {}; }
            void open() override {}
            std::optional<hegadb::server::storage::EncryptedBatch> nextBatch() override { return std::nullopt; }
            void close() override {}
            size_t getRowCount() const override { return 0; }
        };

    public:
        std::unique_ptr<hegadb::server::storage::EncryptedTable> openTable(
            std::string const& table_name) override
        {
            return std::make_unique<EmptyEncryptedTable>(table_name);
        }
    };
    auto storage = std::make_shared<MockStorageManager>();

    auto executor = std::make_shared<hegadb::server::QueryExecutor>(
        catalog, storage, crypto_context);

    return std::make_shared<hegadb::server::ExecuteQueryReceiver>(
        catalog, executor);
}

// ============================================================================
// Encoder
// ============================================================================

TEST(DdlEncoderTest, CreateTableEncodesAllFields)
{
    CreateTableStmt stmt("users");
    stmt.addColumn({ "id", hegadb::types::DataType::Integer, false });
    stmt.addColumn({ "name", hegadb::types::DataType::String, true });
    stmt.addColumn({ "active", hegadb::types::DataType::Boolean, false });
    stmt.addColumn({ "amount", hegadb::types::DataType::Float, true });

    auto request = ExecuteQueryEncoder::encode(stmt, "req-create");

    EXPECT_EQ(request.protocol_version().major(), 1u);
    EXPECT_EQ(request.protocol_version().minor(), 1u);
    EXPECT_EQ(request.request_id().value(), "req-create");

    ASSERT_TRUE(request.statement().has_create_table_statement());
    auto const& create = request.statement().create_table_statement();

    EXPECT_EQ(create.table(), "users");
    ASSERT_EQ(create.columns_size(), 4);

    // Order is preserved and every type is mapped.
    EXPECT_EQ(create.columns(0).name(), "id");
    EXPECT_EQ(create.columns(0).data_type(), proto::DATA_TYPE_INTEGER);
    EXPECT_FALSE(create.columns(0).nullable());

    EXPECT_EQ(create.columns(1).name(), "name");
    EXPECT_EQ(create.columns(1).data_type(), proto::DATA_TYPE_STRING);
    EXPECT_TRUE(create.columns(1).nullable());

    EXPECT_EQ(create.columns(2).name(), "active");
    EXPECT_EQ(create.columns(2).data_type(), proto::DATA_TYPE_BOOLEAN);
    EXPECT_FALSE(create.columns(2).nullable());

    EXPECT_EQ(create.columns(3).name(), "amount");
    EXPECT_EQ(create.columns(3).data_type(), proto::DATA_TYPE_FLOAT);
    EXPECT_TRUE(create.columns(3).nullable());

    // DDL carries no encrypted parameters and no keys.
    EXPECT_EQ(request.parameters_size(), 0);
    EXPECT_FALSE(request.has_keys());
}

TEST(DdlEncoderTest, DropTableEncodesAllFields)
{
    DropTableStmt stmt("users");

    auto request = ExecuteQueryEncoder::encode(stmt, "req-drop");

    EXPECT_EQ(request.protocol_version().major(), 1u);
    EXPECT_EQ(request.protocol_version().minor(), 1u);
    EXPECT_EQ(request.request_id().value(), "req-drop");

    ASSERT_TRUE(request.statement().has_drop_table_statement());
    EXPECT_EQ(request.statement().drop_table_statement().table(), "users");

    EXPECT_EQ(request.parameters_size(), 0);
    EXPECT_FALSE(request.has_keys());
}

TEST(DdlEncoderTest, EncodeDdlDispatchesOnStatementKind)
{
    SQLParser parser;

    auto create = parser.parseToAST("CREATE TABLE t (a INTEGER NOT NULL);");
    ASSERT_NE(create, nullptr);
    EXPECT_TRUE(ExecuteQueryEncoder::encodeDdl(*create, "r1")
            .statement()
            .has_create_table_statement());

    auto drop = parser.parseToAST("DROP TABLE t;");
    ASSERT_NE(drop, nullptr);
    EXPECT_TRUE(ExecuteQueryEncoder::encodeDdl(*drop, "r2")
            .statement()
            .has_drop_table_statement());

    // SELECT must go through query preparation, not encodeDdl.
    auto select = parser.parseToAST("SELECT * FROM t;");
    ASSERT_NE(select, nullptr);
    EXPECT_THROW(ExecuteQueryEncoder::encodeDdl(*select, "r3"),
        std::invalid_argument);
}

// ============================================================================
// Client -> gRPC -> server
// ============================================================================

// Starts a gRPC server running the real ExecuteQueryReceiver and records
// the last request it was given, so tests can check what arrived on the wire.
class DdlGrpcTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        server_ = std::make_unique<hegadb::transport::grpc::GrpcServer>(
            "127.0.0.1:0");

        receiver_ = createTestReceiver();

        server_->setRequestHandler(
            [this](proto::ExecuteQuery const& request) {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    last_request_ = request;
                }
                return receiver_->executeQuery(request);
            });

        server_->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        address_ = server_->address();
    }

    void TearDown() override { server_->shutdown(); }

    std::optional<proto::ExecuteQuery> lastRequest()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_request_;
    }

    std::string address_;
    std::unique_ptr<hegadb::transport::grpc::GrpcServer> server_;
    std::shared_ptr<hegadb::server::ExecuteQueryReceiver> receiver_;

private:
    std::mutex mutex_;
    std::optional<proto::ExecuteQuery> last_request_;
};

TEST_F(DdlGrpcTest, CreateTableReachesServerIntact)
{
    hegadb::transport::grpc::GrpcClient client(address_);
    SQLParser parser;

    auto ast = parser.parseToAST(
        "CREATE TABLE users (id INTEGER NOT NULL, name STRING, "
        "active BOOLEAN NOT NULL, amount FLOAT);");
    ASSERT_NE(ast, nullptr);

    auto response = client.execute(
        ExecuteQueryEncoder::encodeDdl(*ast, "ddl-create-001"));

    // What the server received
    auto received = lastRequest();
    ASSERT_TRUE(received.has_value());
    ASSERT_TRUE(received->statement().has_create_table_statement());

    auto const& create = received->statement().create_table_statement();
    EXPECT_EQ(create.table(), "users");
    ASSERT_EQ(create.columns_size(), 4);
    EXPECT_EQ(create.columns(0).name(), "id");
    EXPECT_EQ(create.columns(0).data_type(), proto::DATA_TYPE_INTEGER);
    EXPECT_FALSE(create.columns(0).nullable());
    EXPECT_EQ(create.columns(1).name(), "name");
    EXPECT_EQ(create.columns(1).data_type(), proto::DATA_TYPE_STRING);
    EXPECT_TRUE(create.columns(1).nullable());
    EXPECT_EQ(create.columns(2).data_type(), proto::DATA_TYPE_BOOLEAN);
    EXPECT_EQ(create.columns(3).data_type(), proto::DATA_TYPE_FLOAT);

    // What the server answered: recognized, but not implemented yet.
    EXPECT_EQ(response.request_id().value(), "ddl-create-001");
    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("not implemented"),
        std::string::npos);
    EXPECT_NE(response.status().message().find("CREATE TABLE"),
        std::string::npos);
}

TEST_F(DdlGrpcTest, DropTableReachesServerIntact)
{
    hegadb::transport::grpc::GrpcClient client(address_);
    SQLParser parser;

    auto ast = parser.parseToAST("DROP TABLE users;");
    ASSERT_NE(ast, nullptr);

    auto response = client.execute(
        ExecuteQueryEncoder::encodeDdl(*ast, "ddl-drop-001"));

    auto received = lastRequest();
    ASSERT_TRUE(received.has_value());
    ASSERT_TRUE(received->statement().has_drop_table_statement());
    EXPECT_EQ(received->statement().drop_table_statement().table(), "users");

    EXPECT_EQ(response.request_id().value(), "ddl-drop-001");
    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("DROP TABLE"),
        std::string::npos);
    EXPECT_NE(response.status().message().find("not implemented"),
        std::string::npos);
}

TEST_F(DdlGrpcTest, SelectStillSucceeds)
{
    hegadb::transport::grpc::GrpcClient client(address_);

    proto::ExecuteQuery request;
    request.mutable_protocol_version()->set_major(1);
    request.mutable_protocol_version()->set_minor(1);
    request.mutable_request_id()->set_value("select-001");

    auto* select = request.mutable_statement()->mutable_select_statement();
    select->mutable_from_table()->set_table("STUDENTS");
    select->add_projections()->mutable_wildcard();

    auto response = client.execute(request);

    EXPECT_EQ(response.status().code(), proto::STATUS_OK);
}

} // namespace
