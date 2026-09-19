#include "GrpcClient.h"
#include "GrpcServer.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace {

class GrpcTest : public ::testing::Test {
protected:
    std::string address_;

    void SetUp() override
    {
        server_ = std::make_unique<hegadb::transport::grpc::GrpcServer>(
            "127.0.0.1:0");

        server_->setRequestHandler(
            [](::hegadb::protocol::ExecuteQuery const& request) {
                ::hegadb::protocol::QueryResponse response;

                *response.mutable_protocol_version() = request.protocol_version();

                *response.mutable_request_id() = request.request_id();

                response.mutable_status()->set_code(
                    ::hegadb::protocol::STATUS_OK);

                response.mutable_status()->set_message(
                    "gRPC transport request received");

                return response;
            });

        server_->start();

        // Give the server a moment to begin listening.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1000));

        address_ = server_->address();
    }

    void TearDown() override
    {
        server_->shutdown();
    }

    std::unique_ptr<hegadb::transport::grpc::GrpcServer> server_;
};

} // namespace

TEST_F(GrpcTest, ExecuteQueryRoundTrip)
{
    using namespace hegadb::protocol;
    using namespace hegadb::transport::grpc;

    GrpcClient client(address_);

    ExecuteQuery request;

    // Protocol version
    request.mutable_protocol_version()->set_major(0);
    request.mutable_protocol_version()->set_minor(6);

    // Request ID
    request.mutable_request_id()->set_value(
        "grpc-test-request-001");

    QueryResponse const response = client.execute(request);

    // Protocol version survives the transport.
    ASSERT_TRUE(response.has_protocol_version());

    EXPECT_EQ(
        response.protocol_version().major(),
        0);

    EXPECT_EQ(
        response.protocol_version().minor(),
        6);

    // Request ID survives the transport.
    ASSERT_TRUE(response.has_request_id());

    EXPECT_EQ(
        response.request_id().value(),
        "grpc-test-request-001");

    // Server successfully processed the RPC.
    ASSERT_TRUE(response.has_status());

    EXPECT_EQ(
        response.status().code(),
        STATUS_OK);

    EXPECT_EQ(
        response.status().message(),
        "gRPC transport request received");
}

TEST(GrpcServerTest, StartsAndShutsDown)
{
    hegadb::transport::grpc::GrpcServer server(
        "127.0.0.1:0");

    EXPECT_NO_THROW(server.start());
    EXPECT_FALSE(server.address().empty());

    EXPECT_NO_THROW(server.shutdown());
}

TEST(GrpcClientTest, ExecuteFailsWhenServerUnavailable)
{
    using namespace hegadb::transport::grpc;
    using namespace hegadb::protocol;

    GrpcClient client("127.0.0.1:0");

    ExecuteQuery request;

    request.mutable_protocol_version()->set_major(0);
    request.mutable_protocol_version()->set_minor(6);

    request.mutable_request_id()->set_value(
        "unavailable-server-test");

    EXPECT_THROW(
        client.execute(request),
        std::runtime_error);
}
