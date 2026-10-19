#include "hegadb/transport/grpc/GrpcClient.h"
#include "hegadb/transport/grpc/GrpcServer.h"

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

TEST(GrpcStreamingTest, MultiBatchServerStreaming)
{
    using namespace hegadb::transport::grpc;
    using namespace hegadb::protocol;

    auto server = std::make_unique<GrpcServer>("127.0.0.1:0");

    server->setStreamingRequestHandler(
        [](ExecuteQuery const& req, hegadb::transport::ServerTransport::BatchWriter writer) {
            for (int i = 0; i < 3; ++i) {
                QueryResponse resp;
                *resp.mutable_protocol_version() = req.protocol_version();
                *resp.mutable_request_id() = req.request_id();
                resp.mutable_status()->set_code(STATUS_OK);
                resp.mutable_result()->set_row_count(10);
                auto* col = resp.mutable_result()->add_columns();
                col->set_column_name("col1");
                col->add_ciphertexts("batch_" + std::to_string(i));
                writer(resp);
            }
        });

    server->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    GrpcClient client(server->address());

    ExecuteQuery request;
    request.mutable_protocol_version()->set_major(1);
    request.mutable_protocol_version()->set_minor(0);
    request.mutable_request_id()->set_value("stream-test-01");

    // Test executeStream receives 3 batches
    int batch_count = 0;
    auto status = client.executeStream(request, [&](QueryResponse const& batch) {
        EXPECT_EQ(batch.status().code(), STATUS_OK);
        EXPECT_EQ(batch.result().row_count(), 10);
        EXPECT_EQ(batch.result().columns(0).column_name(), "col1");
        batch_count++;
        return true;
    });
    EXPECT_EQ(status.code(), STATUS_OK);
    EXPECT_EQ(batch_count, 3);

    // Test aggregated execute combines them
    auto aggregated = client.execute(request);
    EXPECT_EQ(aggregated.status().code(), STATUS_OK);
    EXPECT_EQ(aggregated.result().row_count(), 30);
    ASSERT_EQ(aggregated.result().columns_size(), 1);
    EXPECT_EQ(aggregated.result().columns(0).ciphertexts_size(), 3);

    server->shutdown();
}
