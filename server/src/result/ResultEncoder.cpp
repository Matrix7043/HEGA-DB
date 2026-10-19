#include <hegadb/server/result/ResultEncoder.h>

namespace hegadb::server::result {

void ResultEncoder::encode(
    execution::ExecutionResult const& exec_result,
    protocol::QueryResponse& response) {

    auto* result = response.mutable_result();
    result->set_mode(exec_result.getResultMode());
    result->set_row_count(exec_result.getTotalRowCount());

    // Encode encrypted mask if present
    if (exec_result.hasMask()) {
        auto serialized_mask = exec_result.getEncryptedMask().serialize();
        result->set_encrypted_mask(serialized_mask);
    }

    // Encode all batches
    // Collect columns across batches
    std::map<std::string, std::vector<std::string>> column_data;

    for (auto const& batch : exec_result.getBatches()) {
        for (auto const& col_name : batch.getColumnNames()) {
            auto const& ciphertext = batch.getColumn(col_name);
            column_data[col_name].push_back(ciphertext.serialize());
        }
    }

    // Add to protobuf
    for (auto const& [col_name, ciphertexts] : column_data) {
        auto* col = result->add_columns();
        col->set_column_name(col_name);
        for (auto const& ct : ciphertexts) {
            col->add_ciphertexts(ct);
        }
    }
}

void ResultEncoder::encodeBatch(
    execution::ExecutionBatch const& batch,
    execution::EncryptedMask const* mask,
    protocol::ResultMode mode,
    protocol::QueryResponse& response) {

    auto* result = response.mutable_result();
    result->set_mode(mode);
    result->set_row_count(batch.getRowCount());

    if (mask != nullptr) {
        result->set_encrypted_mask(mask->serialize());
    }

    for (auto const& col_name : batch.getColumnNames()) {
        auto* col = result->add_columns();
        col->set_column_name(col_name);
        col->add_ciphertexts(batch.getColumn(col_name).serialize());
    }
}

} // namespace hegadb::server::result
