#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTParameterizer.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/preparation/PreparedQuery.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/server/ExecuteQueryReceiver.h>
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

    ASSERT_TRUE(select.has_join());
    EXPECT_EQ(select.join().right_table().table(), "orders");
    EXPECT_EQ(select.join().left_column().table(), "users");
    EXPECT_EQ(select.join().left_column().column(), "id");
    EXPECT_EQ(select.join().right_column().table(), "orders");
    EXPECT_EQ(select.join().right_column().column(), "user_id");

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

    EXPECT_FALSE(request.statement().select_statement().has_join());
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

        receiver_ = std::make_shared<hegadb::server::ExecuteQueryReceiver>();

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
    ASSERT_TRUE(received->statement().select_statement().has_join());

    auto const& join = received->statement().select_statement().join();
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
