#include <hegadb/server/result/MaskedRowsResultStrategy.h>

namespace hegadb::server::result {

MaskedRowsResultStrategy::MaskedRowsResultStrategy(
    crypto::OpenFHEContext& crypto_context)
    : crypto_context_(crypto_context) {
}

execution::ExecutionResult MaskedRowsResultStrategy::buildResult(
    std::vector<execution::ExecutionBatch> batches,
    execution::EncryptedMask mask) {

    execution::ExecutionResult result;
    result.setResultMode(protocol::RESULT_MODE_MASKED_ROWS);

    auto const& mask_ct = mask.getCiphertext();
    auto ctx = (mask_ct.getSchemeType() == crypto::SchemeType::BFV)
        ? crypto_context_.getBFVContext()
        : crypto_context_.getCKKSContext();

    // Apply mask to each batch
    for (auto& batch : batches) {
        execution::ExecutionBatch masked_batch;
        masked_batch.setRowCount(batch.getRowCount());

        // Multiply each column by the mask
        for (auto const& col_name : batch.getColumnNames()) {
            auto const& col_ct = batch.getColumn(col_name);
            auto masked = ctx->EvalMult(
                col_ct.getCiphertext(),
                mask_ct.getCiphertext()
            );

            masked_batch.addColumn(
                col_name,
                crypto::OpenFHECiphertext(
                    masked,
                    col_ct.getSchemeType(),
                    col_ct.getOriginalType()
                )
            );
        }

        result.addBatch(std::move(masked_batch));
    }

    return result;
}

} // namespace hegadb::server::result
