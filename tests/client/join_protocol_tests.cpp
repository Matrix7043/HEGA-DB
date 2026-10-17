#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTParameterizer.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/preparation/PreparedQuery.h>
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
#include <string>
#include <thread>

namespace {

using hegadb::client::SQLParser;
using hegadb::client::preparation::PreparedQuery;
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

    // users — used by JoinGrpcTest plain SELECT and join tests
    tables.emplace("users", TableSchema("users", std::vector<ColumnSchema>{
        { "id",   DataType::Integer, false, 0 },
        { "name", DataType::String,  false, 1 },
    }));

    // orders — used by JoinGrpcTest join tests
    tables.emplace("orders", TableSchema("orders", std::vector<ColumnSchema>{
        { "user_id", DataType::Integer, false, 0 },
        { "amount",  DataType::Integer, false, 1 },
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

// A JOIN query has no literals, so it needs no encrypted parameters.
// The query goes through the same parameterizer the client uses.
proto::ExecuteQuery encodeJoin(std::string const& sql, std::string const& id)
{
    SQLParser parser;

    auto ast = parser.parseToAST(sql);
    EXPECT_NE(ast, nullptr);

    hegadb::client::ast::ASTParameterizer parameterizer;
    PreparedQuery prepared(parameterizer.parameterize(*ast), {});

    return ExecuteQueryEncoder::encode(prepared, id);
}

// ============================================================================
// Encoder
// ============================================================================

TEST(JoinEncoderTest, EncodesJoinClause)
{
    auto request = encodeJoin(
        "SELECT users.name, amount FROM users INNER JOIN orders "
        "ON users.id = orders.user_id;",
        "req-join");

    EXPECT_EQ(request.protocol_version().major(), 1u);
    EXPECT_EQ(request.protocol_version().minor(), 1u);
    EXPECT_EQ(request.request_id().value(), "req-join");

    ASSERT_TRUE(request.statement().has_select_statement());
    auto const& select = request.statement().select_statement();

    EXPECT_EQ(select.from_table().table(), "users");
    EXPECT_FALSE(select.has_where());

    ASSERT_TRUE(select.joins_size() > 0);
    EXPECT_EQ(select.joins(0).right_table().table(), "orders");
    EXPECT_EQ(select.joins(0).left_column().table(), "users");
    EXPECT_EQ(select.joins(0).left_column().column(), "id");
    EXPECT_EQ(select.joins(0).right_column().table(), "orders");
    EXPECT_EQ(select.joins(0).right_column().column(), "user_id");

    // Qualified and unqualified projections
    ASSERT_EQ(select.projections_size(), 2);
    EXPECT_EQ(select.projections(0).column_ref().table(), "users");
    EXPECT_EQ(select.projections(0).column_ref().column(), "name");
    EXPECT_EQ(select.projections(1).column_ref().table(), "");
    EXPECT_EQ(select.projections(1).column_ref().column(), "amount");

    EXPECT_EQ(request.parameters_size(), 0);
    EXPECT_FALSE(request.has_keys());
}

TEST(JoinEncoderTest, PlainSelectHasNoJoin)
{
    SQLParser parser;
    auto ast = parser.parseToAST("SELECT name FROM users;");
    ASSERT_NE(ast, nullptr);

    PreparedQuery prepared(std::move(ast), {});
    auto request = ExecuteQueryEncoder::encode(prepared, "req-plain");

    EXPECT_FALSE(request.statement().select_statement().joins_size() > 0);
}

// ============================================================================
// Client -> gRPC -> server
// ============================================================================

class JoinGrpcTest : public ::testing::Test {
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

TEST_F(JoinGrpcTest, JoinReachesServerIntactAndIsNotImplemented)
{
    hegadb::transport::grpc::GrpcClient client(address_);

    auto response = client.execute(encodeJoin(
        "SELECT * FROM users INNER JOIN orders ON users.id = orders.user_id;",
        "join-001"));

    // What the server received
    auto received = lastRequest();
    ASSERT_TRUE(received.has_value());
    ASSERT_TRUE(received->statement().select_statement().joins_size() > 0);

    auto const& join = received->statement().select_statement().joins(0);
    EXPECT_EQ(received->statement().select_statement().from_table().table(), "users");
    EXPECT_EQ(join.right_table().table(), "orders");
    EXPECT_EQ(join.left_column().table(), "users");
    EXPECT_EQ(join.left_column().column(), "id");
    EXPECT_EQ(join.right_column().table(), "orders");
    EXPECT_EQ(join.right_column().column(), "user_id");

    // What the server answered: recognized, but not implemented yet.
    EXPECT_EQ(response.request_id().value(), "join-001");
    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("INNER JOIN"), std::string::npos);
    EXPECT_NE(response.status().message().find("not implemented"), std::string::npos);
    EXPECT_NE(response.status().message().find("users"), std::string::npos);
    EXPECT_NE(response.status().message().find("orders"), std::string::npos);
}

TEST_F(JoinGrpcTest, PlainSelectStillSucceeds)
{
    hegadb::transport::grpc::GrpcClient client(address_);

    SQLParser parser;
    auto ast = parser.parseToAST("SELECT name FROM users;");
    ASSERT_NE(ast, nullptr);

    PreparedQuery prepared(std::move(ast), {});
    auto response = client.execute(
        ExecuteQueryEncoder::encode(prepared, "plain-001"));

    EXPECT_EQ(response.status().code(), proto::STATUS_OK);
}

} // namespace
