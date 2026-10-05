#include <hegadb/execution/ResultDecryptor.h>
#include <hegadb/logging/Logger.h>
#include <hegadb/crypto/Plaintext.h>
#include <hegadb/server/execution/EncryptedMask.h>

#include <sstream>
#include <iomanip>

namespace hegadb::execution {

ResultDecryptor::ResultDecryptor(
    std::shared_ptr<crypto::CompositeEncryption> encryption)
    : encryption_(std::move(encryption))
{
}

DecryptedResult ResultDecryptor::decrypt(
    server::execution::ExecutionResult const& result,
    std::vector<std::string> const& column_names,
    std::vector<types::DataType> const& column_types)
{
    DecryptedResult decrypted;
    decrypted.column_names = column_names;
    decrypted.column_types = column_types;

    logging::debug("ResultDecryptor: Decrypting result with {} columns",
        column_names.size());

    // Get result data
    auto const& batches = result.getBatches();
    size_t row_count = result.getTotalRowCount();

    decrypted.total_rows_scanned = row_count;

    if (row_count == 0 || batches.empty()) {
        logging::debug("ResultDecryptor: No rows to decrypt");
        return decrypted;
    }

    // Decrypt the filter mask
    std::vector<bool> mask;
    if (result.hasMask()) {
        logging::debug("ResultDecryptor: Decrypting filter mask");
        mask = decryptMask(result.getEncryptedMask(), row_count);
    } else {
        // No filter, all rows match
        logging::debug("ResultDecryptor: No filter mask, all rows match");
        mask.resize(row_count, true);
    }

    // Decrypt each column from the first batch
    // TODO: Handle multiple batches properly
    auto const& first_batch = batches[0];
    
    for (size_t col_idx = 0; col_idx < column_names.size(); ++col_idx) {
        auto const& col_name = column_names[col_idx];
        auto const& col_type = column_types[col_idx];

        logging::debug("ResultDecryptor: Decrypting column '{}' (type: {})",
            col_name, static_cast<int>(col_type));

        if (!first_batch.hasColumn(col_name)) {
            throw std::runtime_error("Column not found in result: " + col_name);
        }

        // Decrypt all row slots for this column at once
        auto slot_values = decryptColumnSlots(
            first_batch.getColumn(col_name), col_type, row_count);

        // Distribute slot values across rows
        for (size_t row_idx = 0; row_idx < row_count; ++row_idx) {
            if (decrypted.rows.size() <= row_idx) {
                DecryptedRow row;
                row.matches_filter = mask[row_idx];
                row.values.resize(column_names.size());
                decrypted.rows.push_back(row);
            }
            std::string val = (row_idx < slot_values.size())
                ? slot_values[row_idx]
                : "<missing>";
            decrypted.rows[row_idx].values[col_idx] = val;
        }
    }

    logging::info("ResultDecryptor: Decrypted {} rows ({} matched filter)",
        decrypted.rows.size(),
        std::count_if(mask.begin(), mask.end(), [](bool b) { return b; }));

    return decrypted;
}

std::string ResultDecryptor::decryptAndFormat(
    crypto::OpenFHECiphertext const& ciphertext,
    types::DataType type)
{
    // Decrypt ciphertext
    crypto::Plaintext plaintext = encryption_->decrypt(ciphertext);

    // Format based on type
    std::ostringstream oss;

    // Use std::visit or std::get to extract value from variant
    auto const& value = plaintext.value();

    try {
        switch (type) {
            case types::DataType::Integer:
                if (std::holds_alternative<long long>(value)) {
                    oss << std::get<long long>(value);
                } else {
                    oss << "<type mismatch>";
                }
                break;

            case types::DataType::Float:
                // Float stored as string in our system
                if (std::holds_alternative<std::string>(value)) {
                    oss << std::get<std::string>(value);
                } else {
                    oss << "<type mismatch>";
                }
                break;

            case types::DataType::String:
                if (std::holds_alternative<std::string>(value)) {
                    oss << std::get<std::string>(value);
                } else {
                    oss << "<type mismatch>";
                }
                break;

            case types::DataType::Boolean:
                if (std::holds_alternative<bool>(value)) {
                    oss << (std::get<bool>(value) ? "true" : "false");
                } else {
                    oss << "<type mismatch>";
                }
                break;

            default:
                oss << "<unknown type>";
                break;
        }
    } catch (...) {
        oss << "<decryption error>";
    }

    return oss.str();
}

std::vector<std::string> ResultDecryptor::decryptColumnSlots(
    crypto::OpenFHECiphertext const& ciphertext,
    types::DataType type,
    size_t row_count)
{
    // Reach into the raw OpenFHE ciphertext and decrypt the full slot vector.
    auto const& key_manager = encryption_->getKeyManager();

    lbcrypto::Plaintext pt;
    if (type == types::DataType::Float) {
        // CKKS path
        auto ckks_ctx = encryption_->getContext().getCKKSContext();
        ckks_ctx->Decrypt(key_manager.getCKKSPrivateKey(), ciphertext.getCiphertext(), &pt);
        auto real_vals = pt->GetRealPackedValue();
        std::vector<std::string> result;
        result.reserve(row_count);
        for (size_t i = 0; i < row_count; ++i) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(4) << real_vals[i];
            result.push_back(oss.str());
        }
        return result;
    }

    // BFV path
    auto bfv_ctx = encryption_->getContext().getBFVContext();
    bfv_ctx->Decrypt(key_manager.getBFVPrivateKey(), ciphertext.getCiphertext(), &pt);
    auto packed = pt->GetPackedValue();

    std::vector<std::string> result;
    result.reserve(row_count);

    if (type == types::DataType::Integer || type == types::DataType::Boolean) {
        // One slot per row
        for (size_t i = 0; i < row_count; ++i) {
            int64_t v = (i < packed.size()) ? packed[i] : 0;
            if (type == types::DataType::Boolean) {
                result.push_back(v ? "true" : "false");
            } else {
                result.push_back(std::to_string(v));
            }
        }
    } else if (type == types::DataType::String) {
        // Layout: row_count * max_string_len slots, encoded in stringLength as
        //         row_count * 1000 + max_string_len
        size_t encoded     = ciphertext.getStringLength();
        size_t stored_rows = encoded / 1000;
        size_t stride      = encoded % 1000;

        // Fall back to old scalar encoding (stride == 0 means legacy single string)
        if (stride == 0 || stored_rows == 0) {
            // Legacy: whole string stored as ASCII chars in consecutive slots
            size_t length = encoded; // reused as character count
            std::string s;
            for (size_t c = 0; c < length && c < packed.size(); ++c) {
                s.push_back(static_cast<char>(packed[c]));
            }
            result.resize(row_count, s);
        } else {
            for (size_t row = 0; row < row_count; ++row) {
                std::string s;
                size_t base = row * stride;
                for (size_t c = 0; c < stride; ++c) {
                    int64_t ch = (base + c < packed.size()) ? packed[base + c] : 0;
                    if (ch == 0) break;  // null terminator
                    s.push_back(static_cast<char>(ch));
                }
                result.push_back(s);
            }
        }
    }

    return result;
}

std::vector<bool> ResultDecryptor::decryptMask(
    server::execution::EncryptedMask const& mask,
    size_t row_count)
{
    using MK = server::execution::MaskKind;

    auto const& key_manager = encryption_->getKeyManager();
    auto bfv_ctx = encryption_->getContext().getBFVContext();

    lbcrypto::Plaintext pt;
    bfv_ctx->Decrypt(key_manager.getBFVPrivateKey(),
                     mask.getCiphertext().getCiphertext(), &pt);
    auto packed = pt->GetPackedValue();

    // BFV stores values mod p in [0, p-1].  Values > p/2 represent negatives.
    int64_t p      = static_cast<int64_t>(
        bfv_ctx->GetCryptoParameters()->GetPlaintextModulus());
    int64_t half_p = p / 2;

    auto to_signed = [&](int64_t v) -> int64_t {
        return (v > half_p) ? (v - p) : v;
    };

    MK kind = mask.getKind();

    std::vector<bool> result(row_count);
    for (size_t i = 0; i < row_count; ++i) {
        int64_t raw = (i < packed.size()) ? packed[i] : 0;
        int64_t v   = to_signed(raw);

        switch (kind) {
            // Single-comparison masks: Enc(left - right) per slot
            case MK::DIFF_ZERO:     result[i] = (v == 0);  break;  // EQ
            case MK::DIFF_NONZERO:  result[i] = (v != 0);  break;  // NEQ
            case MK::DIFF_POSITIVE: result[i] = (v >  0);  break;  // GT
            case MK::DIFF_NONNEG:   result[i] = (v >= 0);  break;  // GTE
            case MK::DIFF_NEGATIVE: result[i] = (v <  0);  break;  // LT
            case MK::DIFF_NONPOS:   result[i] = (v <= 0);  break;  // LTE

            // After AND/OR/NOT the server applied BFV arithmetic on top of
            // raw diffs — the result is no longer a clean difference.
            // Best we can do: treat slot == 0 as "not matched" (identity for
            // AND-of-differences where both args were 0; product stays 0).
            case MK::COMPOSED:      result[i] = (v == 0);  break;

            case MK::ALL_MATCH:     result[i] = true;       break;
            default:                result[i] = true;       break;
        }
    }

    logging::info("ResultDecryptor: mask kind={} decoded — {} / {} rows matched",
        static_cast<int>(kind),
        std::count(result.begin(), result.end(), true),
        row_count);

    return result;
}

} // namespace hegadb::execution
