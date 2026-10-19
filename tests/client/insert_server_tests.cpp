#include "hegadb/crypto/CompositeEncryption.h"

#include <hegadb/client/SQLParser.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/transport/grpc/GrpcClient.h>
#include <hegadb/transport/grpc/GrpcServer.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

// What the server does with an INSERT: it recognizes the statement and
// answers that it is not implemented yet.

namespace {

using hegadb::client::SQLParser;
using hegadb::client::protocol::ExecuteQueryEncoder;
namespace proto = hegadb::protocol;

// Parses, encrypts and encodes sql, the way the client does.
proto::ExecuteQuery makeRequest(std::string const& sql, std::string request_id)
{
    static hegadb::crypto::CompositeEncryption encryption;
    static hegadb::client::preparation::QueryPreparer preparer(encryption);

    SQLParser parser;
    auto ast = parser.parseToAST(sql);
    EXPECT_NE(ast, nullptr) << sql;

    auto prepared = preparer.prepare(std::move(ast));
    return ExecuteQueryEncoder::encode(prepared, std::move(request_id));
}

// A receiver with no catalog, storage or crypto context. INSERT, CREATE and
// DROP are answered before the executor touches any of them.
std::shared_ptr<hegadb::server::ExecuteQueryReceiver> makeReceiver()
{
    auto executor = std::make_shared<hegadb::server::QueryExecutor>(
        nullptr, nullptr, nullptr);

    return std::make_shared<hegadb::server::ExecuteQueryReceiver>(
        nullptr, executor);
}

// ============================================================================
// Receiver
// ============================================================================

TEST(InsertReceiverTest, AnswersNotImplemented)
{
    auto receiver = makeReceiver();

    auto response = receiver->executeQuery(
        makeRequest("INSERT INTO users (id, name) VALUES (1, 'alice');", "ins-001"));

    EXPECT_EQ(response.request_id().value(), "ins-001");
    EXPECT_EQ(response.protocol_version().major(), 1u);
    EXPECT_EQ(response.protocol_version().minor(), 1u);

    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);

    auto const& message = response.status().message();
    EXPECT_NE(message.find("INSERT INTO 'users'"), std::string::npos);
    EXPECT_NE(message.find("not implemented"), std::string::npos);

    // An error carries no result.
    EXPECT_FALSE(response.has_result());
}

TEST(InsertReceiverTest, AnswersNotImplementedForAllKindsOfInsert)
{
    auto receiver = makeReceiver();

    for (auto const* sql : {
             "INSERT INTO t VALUES (1);",
             "INSERT INTO t (a, b) VALUES (1, NULL), (2, 'x');",
             "INSERT INTO t VALUES (NULL, NULL);", // no parameters, no keys
         }) {
        auto response = receiver->executeQuery(makeRequest(sql, "ins"));

        EXPECT_EQ(response.status().code(), proto::STATUS_ERROR) << sql;
        EXPECT_NE(response.status().message().find("not implemented"),
            std::string::npos)
            << sql;
    }
}

TEST(InsertReceiverTest, ProtocolVersionIsCheckedFirst)
{
    auto receiver = makeReceiver();

    auto request = makeRequest("INSERT INTO t VALUES (1);", "ins-old");
    request.mutable_protocol_version()->set_major(9);

    auto response = receiver->executeQuery(request);

    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("unsupported protocol version"),
        std::string::npos);
    EXPECT_EQ(response.status().message().find("INSERT"), std::string::npos);
}

TEST(InsertReceiverTest, OtherStatementsAreNotReportedAsInsert)
{
    auto receiver = makeReceiver();

    SQLParser parser;

    auto create = ExecuteQueryEncoder::encodeDdl(
        *parser.parseToAST("CREATE TABLE t (a INTEGER);"), "c");
    auto create_response = receiver->executeQuery(create);
    EXPECT_NE(create_response.status().message().find("CREATE TABLE"), std::string::npos);
    EXPECT_EQ(create_response.status().message().find("INSERT"), std::string::npos);

    auto drop = ExecuteQueryEncoder::encodeDdl(
        *parser.parseToAST("DROP TABLE t;"), "d");
    auto drop_response = receiver->executeQuery(drop);
    EXPECT_NE(drop_response.status().message().find("DROP TABLE"), std::string::npos);
    EXPECT_EQ(drop_response.status().message().find("INSERT"), std::string::npos);
}

// ============================================================================
// Client -> gRPC -> server
// ============================================================================

class InsertGrpcTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        server_ = std::make_unique<hegadb::transport::grpc::GrpcServer>("127.0.0.1:0");

        receiver_ = makeReceiver();

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

TEST_F(InsertGrpcTest, InsertReachesServerIntact)
{
    hegadb::transport::grpc::GrpcClient client(address_);

    auto response = client.execute(makeRequest(
        "INSERT INTO users (id, name) VALUES (1, 'alice'), (2, NULL);", "ins-grpc-001"));

    // What the server received
    auto received = lastRequest();
    ASSERT_TRUE(received.has_value());
    ASSERT_TRUE(received->statement().has_insert_statement());

    auto const& insert = received->statement().insert_statement();
    EXPECT_EQ(insert.table(), "users");
    ASSERT_EQ(insert.columns_size(), 2);
    EXPECT_EQ(insert.columns(0), "id");
    EXPECT_EQ(insert.columns(1), "name");

    ASSERT_EQ(insert.rows_size(), 2);
    EXPECT_EQ(insert.rows(0).values(0).parameter_ref().parameter_index(), 0u);
    EXPECT_EQ(insert.rows(0).values(1).parameter_ref().parameter_index(), 1u);
    EXPECT_EQ(insert.rows(1).values(0).parameter_ref().parameter_index(), 2u);
    EXPECT_TRUE(insert.rows(1).values(1).has_null_value());

    // Three encrypted parameters and the keys arrived too.
    EXPECT_EQ(received->parameters_size(), 3);
    EXPECT_TRUE(received->has_keys());

    // The statement itself carries no plaintext.
    EXPECT_EQ(received->statement().SerializeAsString().find("alice"),
        std::string::npos);

    // What the server answered: recognized, but not implemented yet.
    EXPECT_EQ(response.request_id().value(), "ins-grpc-001");
    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), proto::STATUS_ERROR);
    EXPECT_NE(response.status().message().find("INSERT INTO 'users'"), std::string::npos);
    EXPECT_NE(response.status().message().find("not implemented"), std::string::npos);
}

} // namespace
