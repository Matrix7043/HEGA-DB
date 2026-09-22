#include "hegadb/transport/grpc/GrpcClient.h"
#include "hegadb/transport/grpc/GrpcServer.h"
#include <hegadb/server/ExecuteQueryReceiver.h>

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace {

constexpr uint32_t TEST_MAJOR_VERSION = 1;
constexpr uint32_t TEST_MINOR_VERSION = 1;

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
    hegadb::server::ExecuteQueryReceiver receiver;
    auto query = createValidQuery("test-valid-01");

    auto response = receiver.executeQuery(query);

    // Assert version & request ID preservation
    EXPECT_EQ(response.protocol_version().major(), TEST_MAJOR_VERSION);
    EXPECT_EQ(response.protocol_version().minor(), TEST_MINOR_VERSION);
    EXPECT_EQ(response.request_id().value(), "test-valid-01");

    // Assert status OK
    ASSERT_TRUE(response.has_status());
    EXPECT_EQ(response.status().code(), hegadb::protocol::STATUS_OK);
}

TEST(ExecuteQueryReceiverTest, RejectsProtocolVersionMismatch)
{
    hegadb::server::ExecuteQueryReceiver receiver;
    auto query = createValidQuery("test-version-mismatch");

    // Incompatible version (9.9)
    query.mutable_protocol_version()->set_major(9);
    query.mutable_protocol_version()->set_minor(9);

    auto response = receiver.executeQuery(query);

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
        receiver_ = std::make_shared<hegadb::server::ExecuteQueryReceiver>();

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
