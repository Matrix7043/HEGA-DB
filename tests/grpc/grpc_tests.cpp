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

        server_->Start();

        // Give the server a moment to begin listening.
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1000));

        address_ = server_->address();
    }

    void TearDown() override
    {
        server_->Shutdown();
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

    QueryResponse const response = client.Execute(request);

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
