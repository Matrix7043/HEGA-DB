#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/KeyManager.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/Plaintext.h>
#include <openfhe.h>

#include <cassert>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

// Helper to decrypt the full slot vector from a BFV ciphertext
std::vector<int64_t> decryptRawSlots(
    hegadb::crypto::CompositeEncryption const& encryption,
    hegadb::crypto::OpenFHECiphertext const& ct)
{
    auto bfv_ctx = encryption.getContext().getBFVContext();
    auto priv_key = encryption.getKeyManager().getBFVPrivateKey();

    lbcrypto::Plaintext pt;
    bfv_ctx->Decrypt(priv_key, ct.getCiphertext(), &pt);

    int64_t p = static_cast<int64_t>(bfv_ctx->GetCryptoParameters()->GetPlaintextModulus());
    int64_t half_p = p / 2;

    std::vector<int64_t> raw = pt->GetPackedValue();
    std::vector<int64_t> signed_values;
    signed_values.reserve(raw.size());

    for (int64_t val : raw) {
        signed_values.push_back((val > half_p) ? (val - p) : val);
    }
    return signed_values;
}

void printSlots(std::string const& label, std::vector<int64_t> const& slots, size_t count = 8)
{
    std::cout << label << " [ ";
    for (size_t i = 0; i < count && i < slots.size(); ++i) {
        std::cout << slots[i] << " ";
    }
    std::cout << "... (" << slots.size() << " total slots) ]\n";
}

} // namespace

int main()
{
    std::cout << "========================================================\n";
    std::cout << " HEGA-DB: Single-Slot (encryptInsert) & Append Test    \n";
    std::cout << "========================================================\n\n";

    // Use isolated temporary directory for test keys
    std::string test_key_dir = "/tmp/hegadb_insert_test_" + std::to_string(std::time(nullptr));
    std::filesystem::create_directories(test_key_dir);

    std::cout << "1. Initializing CompositeEncryption with test directory: " << test_key_dir << "\n";
    hegadb::crypto::CompositeEncryption encryption(test_key_dir);

    auto bfv_ctx = encryption.getContext().getBFVContext();
    auto priv_key = encryption.getKeyManager().getBFVPrivateKey();

    // Generate rotation keys for powers/steps if server rotation is performed
    std::vector<int32_t> rot_indices = {1, 2, 3, 4, 5, 6, 7, 8, -1, -2, -3, -4, -5, -6, -7, -8};
    bfv_ctx->EvalRotateKeyGen(priv_key, rot_indices);

    // =========================================================================
    // Test Case 1: Client encrypts insert value 15 using encryptInsert
    // =========================================================================
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "[TEST 1] Client encrypts scalar 15 with encryptInsert\n";
    std::cout << "--------------------------------------------------------\n";

    hegadb::crypto::Plaintext pt_val(15LL);
    auto insert_ct = encryption.encryptInsert(pt_val);
    auto* openfhe_insert_ct = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(insert_ct.get());
    assert(openfhe_insert_ct != nullptr);

    auto slots_after_enc = decryptRawSlots(encryption, *openfhe_insert_ct);
    printSlots("Decrypted slots after encryptInsert(15):", slots_after_enc);

    // Verify slot 0 is 15 and remaining slots are 0
    assert(slots_after_enc[0] == 15);
    for (size_t i = 1; i < 8; ++i) {
        assert(slots_after_enc[i] == 0);
    }
    std::cout << "--> PASS: Slot 0 is 15 and subsequent slots are all 0!\n";

    // =========================================================================
    // Test Case 2: Wire Serialization & Deserialization
    // =========================================================================
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "[TEST 2] Wire Serialization & Deserialization\n";
    std::cout << "--------------------------------------------------------\n";

    std::string wire_bytes = openfhe_insert_ct->serialize();
    std::cout << "Serialized wire size: " << wire_bytes.size() << " bytes\n";
    std::cout << "Magic bytes: " << wire_bytes.substr(0, 4) << "\n";
    assert(wire_bytes.size() > 15);

    auto deserialized_ct = hegadb::crypto::OpenFHECiphertext::deserialize(wire_bytes);
    assert(deserialized_ct.getSchemeType() == hegadb::crypto::SchemeType::BFV);
    assert(deserialized_ct.getOriginalType() == hegadb::types::DataType::Integer);

    auto slots_after_wire = decryptRawSlots(encryption, deserialized_ct);
    printSlots("Decrypted slots after deserialize:", slots_after_wire);
    assert(slots_after_wire[0] == 15);
    assert(slots_after_wire[1] == 0);
    assert(slots_after_wire[2] == 0);
    std::cout << "--> PASS: Wire deserialization preserved slot 0=15 and zero-padding!\n";

    // Standard decrypt() check
    auto standard_decrypted = encryption.decrypt(deserialized_ct);
    assert(std::get<long long>(standard_decrypted.value()) == 15LL);
    std::cout << "--> PASS: Standard decrypt() returned 15LL.\n";

    // =========================================================================
    // Test Case 3: Server-side Rotation & In-Place Column Addition
    // =========================================================================
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "[TEST 3] Server Simulation: Appending 15 to empty Slot 3\n";
    std::cout << "--------------------------------------------------------\n";

    // Simulate an existing database column with 3 occupied rows: [10, 20, 30, 0, 0, ...]
    uint32_t ring_dim = bfv_ctx->GetRingDimension();
    std::vector<int64_t> initial_column(ring_dim, 0);
    initial_column[0] = 10;
    initial_column[1] = 20;
    initial_column[2] = 30;
    // Slot 3 is the first empty slot!

    auto initial_pt = bfv_ctx->MakePackedPlaintext(initial_column);
    auto pub_key = encryption.getKeyManager().getBFVPublicKey();
    auto column_ct = bfv_ctx->Encrypt(pub_key, initial_pt);

    hegadb::crypto::OpenFHECiphertext col_wrapper(column_ct, hegadb::crypto::SchemeType::BFV, hegadb::types::DataType::Integer);
    printSlots("Server initial column state:", decryptRawSlots(encryption, col_wrapper));

    // Server operation:
    // Step 1: Rotate the client's single-slot ciphertext by -3 (shifts slot 0 -> slot 3)
    int32_t target_slot = 3;
    auto rotated_insert_ct = bfv_ctx->EvalRotate(deserialized_ct.getCiphertext(), -target_slot);

    hegadb::crypto::OpenFHECiphertext rotated_wrapper(rotated_insert_ct, hegadb::crypto::SchemeType::BFV, hegadb::types::DataType::Integer);
    printSlots("Client ciphertext rotated by -3:", decryptRawSlots(encryption, rotated_wrapper));
    assert(decryptRawSlots(encryption, rotated_wrapper)[3] == 15);
    assert(decryptRawSlots(encryption, rotated_wrapper)[0] == 0);

    // Step 2: Homomorphic addition (col_ct + rotated_insert_ct)
    auto updated_column_ct = bfv_ctx->EvalAdd(column_ct, rotated_insert_ct);

    hegadb::crypto::OpenFHECiphertext updated_col_wrapper(updated_column_ct, hegadb::crypto::SchemeType::BFV, hegadb::types::DataType::Integer);
    auto final_slots = decryptRawSlots(encryption, updated_col_wrapper);
    printSlots("Server updated column state after EvalAdd:", final_slots);

    // Verify all 4 elements
    assert(final_slots[0] == 10);
    assert(final_slots[1] == 20);
    assert(final_slots[2] == 30);
    assert(final_slots[3] == 15); // Newly inserted!
    assert(final_slots[4] == 0);  // Still empty

    std::cout << "--> PASS: Server successfully appended 15 into Slot 3 purely in ciphertext!\n";

    // =========================================================================
    // Test Case 4: Negative Integer Support
    // =========================================================================
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "[TEST 4] Single-slot encryption with negative value (-42)\n";
    std::cout << "--------------------------------------------------------\n";

    hegadb::crypto::Plaintext pt_neg(-42LL);
    auto neg_ct = encryption.encryptInsert(pt_neg);
    auto* openfhe_neg_ct = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(neg_ct.get());

    auto neg_slots = decryptRawSlots(encryption, *openfhe_neg_ct);
    printSlots("Decrypted slots for -42:", neg_slots);
    assert(neg_slots[0] == -42);
    assert(neg_slots[1] == 0);
    assert(std::get<long long>(encryption.decrypt(*neg_ct).value()) == -42LL);
    std::cout << "--> PASS: Negative integer (-42) correctly preserved at slot 0!\n";

    // =========================================================================
    // Test Case 5: Boolean Support
    // =========================================================================
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "[TEST 5] Single-slot encryption with boolean (true)\n";
    std::cout << "--------------------------------------------------------\n";

    hegadb::crypto::Plaintext pt_bool(true);
    auto bool_ct = encryption.encryptInsert(pt_bool);
    auto* openfhe_bool_ct = dynamic_cast<hegadb::crypto::OpenFHECiphertext*>(bool_ct.get());

    auto bool_slots = decryptRawSlots(encryption, *openfhe_bool_ct);
    printSlots("Decrypted slots for boolean true:", bool_slots);
    assert(bool_slots[0] == 1);
    assert(bool_slots[1] == 0);
    assert(std::get<bool>(encryption.decrypt(*bool_ct).value()) == true);
    std::cout << "--> PASS: Boolean (true -> 1) correctly preserved at slot 0!\n";

    // Cleanup test keys
    std::filesystem::remove_all(test_key_dir);

    std::cout << "\n========================================================\n";
    std::cout << " ALL TESTS PASSED SUCCESSFULLY!                         \n";
    std::cout << "========================================================\n";
    return 0;
}
