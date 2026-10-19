#include "hegadb/transport/grpc/GrpcClient.h"
#include "query.pb.h"
#include "service.grpc.pb.h"

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/status.h>
#include <stdexcept>
#include <string>

namespace hegadb::transport::grpc {

namespace {
    /*
     * A single serialized BFV ciphertext is ~1 MiB; a full QueryResponse can
     * carry several columns × several segments, so the default 4 MiB receive
     * limit is easily exceeded.  Match the server's 64 MiB send limit.
     */
    constexpr int MAX_MESSAGE_BYTES = 64 * 1024 * 1024;
} // namespace

GrpcClient::GrpcClient(std::string const& address)
{
    ::grpc::ChannelArguments args;
    args.SetMaxReceiveMessageSize(MAX_MESSAGE_BYTES);
    args.SetMaxSendMessageSize(MAX_MESSAGE_BYTES);

    stub_ = HEGADBService::NewStub(
        ::grpc::CreateCustomChannel(
            address,
            ::grpc::InsecureChannelCredentials(),
            args));
}

protocol::Status GrpcClient::executeStream(
    protocol::ExecuteQuery const& request,
    BatchCallback callback)
{
    ::grpc::ClientContext context;
    std::unique_ptr<::grpc::ClientReader<protocol::QueryResponse>> reader(
        stub_->Execute(&context, request));

    protocol::QueryResponse batch;
    protocol::Status last_status;

    while (reader->Read(&batch)) {
        if (batch.has_status()) {
            last_status = batch.status();
        }
        if (!callback(batch)) {
            context.TryCancel();
            break;
        }
    }

    ::grpc::Status const status = reader->Finish();
    if (!status.ok()) {
        throw std::runtime_error(
            "gRPC Execute stream failed: " + status.error_message());
    }

    return last_status;
}

protocol::QueryResponse GrpcClient::execute(
    protocol::ExecuteQuery const& request)
{
    protocol::QueryResponse aggregated;
    bool is_first = true;

    executeStream(request, [&](protocol::QueryResponse const& batch) -> bool {
        if (is_first) {
            if (batch.has_protocol_version()) {
                *aggregated.mutable_protocol_version() = batch.protocol_version();
            }
            if (batch.has_request_id()) {
                *aggregated.mutable_request_id() = batch.request_id();
            }
            if (batch.has_status()) {
                *aggregated.mutable_status() = batch.status();
            }
            if (batch.has_result()) {
                aggregated.mutable_result()->set_mode(batch.result().mode());
                aggregated.mutable_result()->set_row_count(batch.result().row_count());
                if (!batch.result().encrypted_mask().empty()) {
                    aggregated.mutable_result()->set_encrypted_mask(batch.result().encrypted_mask());
                }
            }
            is_first = false;
        } else {
            if (batch.has_status() && batch.status().code() != protocol::STATUS_OK) {
                *aggregated.mutable_status() = batch.status();
            }
            if (batch.has_result()) {
                aggregated.mutable_result()->set_row_count(
                    aggregated.result().row_count() + batch.result().row_count());
                if (aggregated.result().encrypted_mask().empty() && !batch.result().encrypted_mask().empty()) {
                    aggregated.mutable_result()->set_encrypted_mask(batch.result().encrypted_mask());
                }
            }
        }

        if (batch.has_result()) {
            for (auto const& col : batch.result().columns()) {
                protocol::EncryptedColumn* target_col = nullptr;
                for (int i = 0; i < aggregated.result().columns_size(); ++i) {
                    if (aggregated.result().columns(i).column_name() == col.column_name()) {
                        target_col = aggregated.mutable_result()->mutable_columns(i);
                        break;
                    }
                }
                if (!target_col) {
                    target_col = aggregated.mutable_result()->add_columns();
                    target_col->set_column_name(col.column_name());
                }
                for (auto const& ct : col.ciphertexts()) {
                    target_col->add_ciphertexts(ct);
                }
            }
        }

        return true;
    });

    return aggregated;
}

}
