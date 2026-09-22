#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/MockCiphertext.h>
#include <hegadb/crypto/MockEncryption.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/crypto/Plaintext.h>

#include "query.pb.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

/*
 * Serialized layout under test (see OpenFHECiphertext.cpp):
 *
 *   [0..3]   magic "HGCT"
 *   [4]      format version
 *   [5]      scheme type
 *   [6]      original data type
 *   [7..14]  string length (little-endian)
 *   [15..]   OpenFHE binary serialization of the ciphertext
 */
static constexpr std::size_t HEADER_SIZE = 15;

// Encrypt -> serialize -> deserialize -> decrypt, as the two ends of the
// protocol would do it.
static hegadb::crypto::Plaintext roundTrip(
    hegadb::crypto::CompositeEncryption& encryption,
    hegadb::crypto::Plaintext const& value)
{
    auto ciphertext = encryption.encrypt(value);
    auto restored = hegadb::crypto::OpenFHECiphertext::deserialize(ciphertext->serialize());

    return encryption.decrypt(restored);
}

// True if deserialize() rejects these bytes with a std::runtime_error.
static bool isRejected(std::string const& bytes)
{
    bool failed = false;

    try {
        hegadb::crypto::OpenFHECiphertext::deserialize(bytes);
    } catch (std::runtime_error const&) {
        failed = true;
    }

    return failed;
}

int main()
{
    std::cout
        << "========================================\n"
        << " HEGADB Crypto Serialization Tests\n"
        << "========================================\n\n";

    std::cout << "Initializing OpenFHE encryption...\n";
    std::cout << "(This may take 2-5 seconds for key generation)\n\n";

    hegadb::crypto::CompositeEncryption encryption;

    // ========================================
    // Test 1: Integer round trip
    // ========================================

    std::cout << "[TEST 1] Integer round trip...\n";

    {
        auto decrypted = roundTrip(encryption, hegadb::crypto::Plaintext(42LL));

        assert(decrypted.type() == hegadb::types::DataType::Integer);
        assert(std::get<long long>(decrypted.value()) == 42);

        std::cout << "  PASS: 42 -> encrypted -> serialized -> deserialized -> 42\n";
    }

    // ========================================
    // Test 2: Negative integer round trip
    // ========================================

    std::cout << "[TEST 2] Negative integer round trip...\n";

    {
        auto decrypted = roundTrip(encryption, hegadb::crypto::Plaintext(-100LL));

        assert(std::get<long long>(decrypted.value()) == -100);

        std::cout << "  PASS: -100 survives serialization\n";
    }

    // ========================================
    // Test 3: String round trip
    // ========================================

    std::cout << "[TEST 3] String round trip...\n";

    {
        auto decrypted = roundTrip(
            encryption, hegadb::crypto::Plaintext(std::string("Hello World")));

        assert(decrypted.type() == hegadb::types::DataType::String);
        assert(std::get<std::string>(decrypted.value()) == "Hello World");

        std::cout << "  PASS: \"Hello World\" survives serialization\n";
    }

    // ========================================
    // Test 4: Empty string round trip
    // ========================================

    std::cout << "[TEST 4] Empty string round trip...\n";

    {
        auto decrypted = roundTrip(
            encryption, hegadb::crypto::Plaintext(std::string("")));

        assert(std::get<std::string>(decrypted.value()) == "");

        std::cout << "  PASS: \"\" survives serialization\n";
    }

    // ========================================
    // Test 5: Boolean round trip
    // ========================================

    std::cout << "[TEST 5] Boolean round trip...\n";

    {
        auto decryptedTrue = roundTrip(encryption, hegadb::crypto::Plaintext(true));
        auto decryptedFalse = roundTrip(encryption, hegadb::crypto::Plaintext(false));

        assert(decryptedTrue.type() == hegadb::types::DataType::Boolean);
        assert(std::get<bool>(decryptedTrue.value()) == true);
        assert(std::get<bool>(decryptedFalse.value()) == false);

        std::cout << "  PASS: true and false survive serialization\n";
    }

    // ========================================
    // Test 6: Metadata survives serialization
    // ========================================

    std::cout << "[TEST 6] Metadata survives serialization...\n";

    {
        auto original = encryption.encrypt(
            hegadb::crypto::Plaintext(std::string("alice")));

        auto const* before = dynamic_cast<hegadb::crypto::OpenFHECiphertext const*>(original.get());
        assert(before != nullptr);

        auto restored = hegadb::crypto::OpenFHECiphertext::deserialize(original->serialize());

        assert(restored.getSchemeType() == before->getSchemeType());
        assert(restored.getSchemeType() == hegadb::crypto::SchemeType::BFV);
        assert(restored.getOriginalType() == before->getOriginalType());
        assert(restored.getOriginalType() == hegadb::types::DataType::String);
        assert(restored.getStringLength() == before->getStringLength());
        assert(restored.getStringLength() == 5);

        std::cout << "  PASS: scheme type, data type and string length preserved\n";
    }

    // ========================================
    // Test 7: CKKS round trip
    // ========================================

    // CKKSEncryption::encrypt is not implemented yet (Variant has no double),
    // so the CKKS ciphertext is built directly with OpenFHE.

    std::cout << "[TEST 7] CKKS round trip...\n";

    {
        auto ckksContext = encryption.getContext().getCKKSContext();

        auto plaintext = ckksContext->MakeCKKSPackedPlaintext(std::vector<double> { 3.14159 });
        auto ciphertext = ckksContext->Encrypt(
            encryption.getKeyManager().getCKKSPublicKey(), plaintext);

        hegadb::crypto::OpenFHECiphertext original(
            ciphertext,
            hegadb::crypto::SchemeType::CKKS,
            hegadb::types::DataType::Float);

        auto restored = hegadb::crypto::OpenFHECiphertext::deserialize(original.serialize());

        assert(restored.getSchemeType() == hegadb::crypto::SchemeType::CKKS);
        assert(restored.getOriginalType() == hegadb::types::DataType::Float);

        lbcrypto::Plaintext decrypted;
        ckksContext->Decrypt(
            encryption.getKeyManager().getCKKSPrivateKey(),
            restored.getCiphertext(),
            &decrypted);
        decrypted->SetLength(1);

        assert(std::abs(decrypted->GetRealPackedValue()[0] - 3.14159) < 1e-6);

        std::cout << "  PASS: 3.14159 -> encrypted -> serialized -> deserialized -> 3.14159\n";
    }

    // ========================================
    // Test 8: Wire header layout
    // ========================================

    std::cout << "[TEST 8] Wire header layout...\n";

    {
        auto ciphertext = encryption.encrypt(hegadb::crypto::Plaintext(42LL));
        auto bytes = ciphertext->serialize();

        assert(bytes.size() > HEADER_SIZE);
        assert(bytes.compare(0, 4, "HGCT") == 0);
        assert(static_cast<unsigned char>(bytes[4]) == 1); // format version
        assert(bytes[5] == static_cast<char>(hegadb::crypto::SchemeType::BFV));
        assert(bytes[6] == static_cast<char>(hegadb::types::DataType::Integer));

        for (std::size_t i = 7; i < HEADER_SIZE; ++i) {
            assert(bytes[i] == 0); // string length is 0 for an integer
        }

        // Ciphertexts are large; this matters for gRPC's message size limit.
        std::cout
            << "  INFO: one BFV integer ciphertext = " << bytes.size() << " bytes ("
            << (bytes.size() / 1024.0 / 1024.0) << " MiB)\n";

        std::cout << "  PASS: header is HGCT | version | scheme | type | string length\n";
    }

    // ========================================
    // Test 9: Ciphertexts through a protobuf ExecuteQuery
    // ========================================

    std::cout << "[TEST 9] Ciphertexts through protobuf ExecuteQuery...\n";

    {
        hegadb::protocol::ExecuteQuery request;

        auto* first = request.add_parameters();
        first->set_index(0);
        first->set_ciphertext(
            encryption.encrypt(hegadb::crypto::Plaintext(42LL))->serialize());

        auto* second = request.add_parameters();
        second->set_index(1);
        second->set_ciphertext(
            encryption.encrypt(hegadb::crypto::Plaintext(std::string("alice")))->serialize());

        // What actually crosses the network:
        std::string wireBytes;
        [[maybe_unused]] bool const serialized = request.SerializeToString(&wireBytes);
        assert(serialized);

        hegadb::protocol::ExecuteQuery received;
        [[maybe_unused]] bool const parsed = received.ParseFromString(wireBytes);
        assert(parsed);

        assert(received.parameters_size() == 2);
        assert(received.parameters(0).index() == 0);
        assert(received.parameters(1).index() == 1);

        auto restoredFirst = hegadb::crypto::OpenFHECiphertext::deserialize(
            received.parameters(0).ciphertext());
        auto restoredSecond = hegadb::crypto::OpenFHECiphertext::deserialize(
            received.parameters(1).ciphertext());

        assert(std::get<long long>(encryption.decrypt(restoredFirst).value()) == 42);
        assert(std::get<std::string>(encryption.decrypt(restoredSecond).value()) == "alice");

        std::cout << "  PASS: 2 ciphertexts survived ExecuteQuery serialize/parse ("
                  << wireBytes.size() << " byte message)\n";
    }

    // ========================================
    // Test 10: Garbage input is rejected
    // ========================================

    // Deserialization input comes from the network, so bad bytes must raise
    // a std::runtime_error rather than crash or return a bogus ciphertext.

    std::cout << "[TEST 10] Garbage input is rejected...\n";

    {
        auto valid = encryption.encrypt(hegadb::crypto::Plaintext(42LL))->serialize();

        assert(isRejected(""));
        assert(isRejected("HG"));
        assert(isRejected("not a ciphertext at all"));

        auto wrongMagic = valid;
        wrongMagic[0] = 'X';
        assert(isRejected(wrongMagic));

        std::cout << "  PASS: empty, too short, not a ciphertext and bad magic all rejected\n";
    }

    // ========================================
    // Test 11: Invalid header fields are rejected
    // ========================================

    std::cout << "[TEST 11] Invalid header fields are rejected...\n";

    {
        auto valid = encryption.encrypt(hegadb::crypto::Plaintext(42LL))->serialize();

        auto badVersion = valid;
        badVersion[4] = 99;
        assert(isRejected(badVersion));

        auto badScheme = valid;
        badScheme[5] = 9;
        assert(isRejected(badScheme));

        auto badType = valid;
        badType[6] = 9;
        assert(isRejected(badType));

        // Claims CKKS while the data type is Integer.
        auto schemeMismatch = valid;
        schemeMismatch[5] = static_cast<char>(hegadb::crypto::SchemeType::CKKS);
        assert(isRejected(schemeMismatch));

        // Claims a string length on a non-string.
        auto badLength = valid;
        badLength[7] = 5;
        assert(isRejected(badLength));

        std::cout << "  PASS: unknown version/scheme/type and inconsistent metadata rejected\n";
    }

    // ========================================
    // Test 12: Damaged payload is rejected
    // ========================================

    std::cout << "[TEST 12] Damaged payload is rejected...\n";

    {
        auto valid = encryption.encrypt(hegadb::crypto::Plaintext(42LL))->serialize();

        assert(isRejected(valid.substr(0, HEADER_SIZE)));

        assert(isRejected(valid.substr(0, valid.size() / 2)));

        auto zeroed = valid;
        for (std::size_t i = HEADER_SIZE; i < zeroed.size(); ++i) {
            zeroed[i] = 0;
        }
        assert(isRejected(zeroed));

        std::cout << "  PASS: header-only, truncated and zeroed payloads rejected\n";
    }

    // ========================================
    // Test 13: Empty ciphertext cannot be serialized
    // ========================================

    std::cout << "[TEST 13] Empty ciphertext cannot be serialized...\n";

    {
        hegadb::crypto::OpenFHECiphertext empty(
            nullptr,
            hegadb::crypto::SchemeType::BFV,
            hegadb::types::DataType::Integer);

        bool failed = false;

        try {
            empty.serialize();
        } catch (std::runtime_error const&) {
            failed = true;
        }

        assert(failed);

        std::cout << "  PASS: null OpenFHE ciphertext throws\n";
    }

    // ========================================
    // Test 14: MockCiphertext serialization
    // ========================================

    // The protocol tests use MockCiphertext, so it must serialize too.

    std::cout << "[TEST 14] MockCiphertext serialization...\n";

    {
        hegadb::crypto::MockEncryption mockEncryption;

        auto integer = mockEncryption.encrypt(hegadb::crypto::Plaintext(42LL));
        auto text = mockEncryption.encrypt(hegadb::crypto::Plaintext(std::string("Alice")));
        auto flag = mockEncryption.encrypt(hegadb::crypto::Plaintext(true));

        assert(integer->serialize() == "MOCK:integer:42");
        assert(text->serialize() == "MOCK:string:Alice");
        assert(flag->serialize() == "MOCK:boolean:true");

        // Same value -> same bytes; different value -> different bytes.
        auto again = mockEncryption.encrypt(hegadb::crypto::Plaintext(42LL));
        auto other = mockEncryption.encrypt(hegadb::crypto::Plaintext(43LL));

        assert(integer->serialize() == again->serialize());
        assert(integer->serialize() != other->serialize());

        // Mock bytes are not a real ciphertext and must never be accepted as one.
        assert(isRejected(integer->serialize()));

        std::cout << "  PASS: mock bytes are readable, deterministic and rejected by OpenFHE deserialize\n";
    }

    // ========================================
    // Test 15: Types without serialization support
    // ========================================

    std::cout << "[TEST 15] Types without serialization support...\n";

    {
        // A ciphertext type that does not override serialize().
        struct UnserializableCiphertext final : hegadb::crypto::Ciphertext {
        };

        UnserializableCiphertext unserializable;

        bool failed = false;

        try {
            unserializable.serialize();
        } catch (std::runtime_error const&) {
            failed = true;
        }

        assert(failed);

        std::cout << "  PASS: the base Ciphertext::serialize default throws\n";
    }

    // ========================================
    // Test 16: Deserialize without a matching context
    // ========================================

    // Keep this test last: it clears OpenFHE's process-wide context registry.

    std::cout << "[TEST 16] Deserialize without a matching context...\n";

    {
        auto const expectedRingDimension
            = encryption.getContext().getBFVContext()->GetRingDimension();

        auto bytes = encryption.encrypt(hegadb::crypto::Plaintext(42LL))->serialize();

        // Simulate a fresh server process: forget every registered context.
        lbcrypto::CryptoContextFactory<lbcrypto::DCRTPoly>::ReleaseAllContexts();

        auto restored = hegadb::crypto::OpenFHECiphertext::deserialize(bytes);
        auto restoredContext = restored.getCiphertext()->GetCryptoContext();

        assert(restoredContext != nullptr);
        assert(restoredContext->GetRingDimension() == expectedRingDimension);

        std::cout << "  PASS: context rebuilt from the parameters embedded in the bytes\n";
    }

    std::cout
        << "\n========================================\n"
        << " All serialization tests passed! (15/15)\n"
        << "========================================\n";

    return 0;
}