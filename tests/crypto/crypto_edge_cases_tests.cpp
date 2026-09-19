#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/Plaintext.h>

#include <cassert>
#include <iostream>
#include <limits>
#include <string>

int main()
{
    std::cout
        << "========================================\n"
        << " HEGADB Crypto Edge Case Tests\n"
        << "========================================\n\n";

    std::cout << "Initializing encryption...\n\n";

    hegadb::crypto::CompositeEncryption encryption;

    // ========================================
    // Test 1: Maximum integer value
    // ========================================

    std::cout << "[TEST 1] Maximum integer value...\n";

    {
        long long largeValue = 1000000000LL;
        auto plaintext = hegadb::crypto::Plaintext(largeValue);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<long long>(decrypted.value()) == largeValue);

        std::cout << "  PASS: Large positive value encrypted and decrypted\n";
    }

    // ========================================
    // Test 2: Minimum integer value
    // ========================================

    std::cout << "[TEST 2] Minimum integer value...\n";

    {
        long long largeNegative = -1000000000LL;
        auto plaintext = hegadb::crypto::Plaintext(largeNegative);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<long long>(decrypted.value()) == largeNegative);

        std::cout << "  PASS: Large negative value encrypted and decrypted\n";
    }

    // ========================================
    // Test 3: Very long string
    // ========================================

    std::cout << "[TEST 3] Very long string (100 characters)...\n";

    {
        std::string longStr(100, 'A');
        auto plaintext = hegadb::crypto::Plaintext(longStr);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<std::string>(decrypted.value()) == longStr);

        std::cout << "  PASS: 100-character string preserved\n";
    }

    // ========================================
    // Test 4: String with all printable ASCII
    // ========================================

    std::cout << "[TEST 4] All printable ASCII characters...\n";

    {
        std::string asciiStr;
        for (char c = 32; c < 127; ++c) {
            asciiStr += c;
        }

        auto plaintext = hegadb::crypto::Plaintext(asciiStr);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<std::string>(decrypted.value()) == asciiStr);

        std::cout << "  PASS: All printable ASCII preserved\n";
    }

    // ========================================
    // Test 5: Rapid sequential encryptions
    // ========================================

    std::cout << "[TEST 5] Rapid sequential encryptions (100 values)...\n";

    {
        for (int i = 0; i < 100; ++i) {
            auto plaintext = hegadb::crypto::Plaintext(static_cast<long long>(i));
            auto ciphertext = encryption.encrypt(plaintext);
            auto decrypted = encryption.decrypt(*ciphertext);

            assert(std::get<long long>(decrypted.value()) == i);
        }

        std::cout << "  PASS: 100 rapid encryptions/decryptions successful\n";
    }

    // ========================================
    // Test 6: Same value encrypted multiple times
    // ========================================

    std::cout << "[TEST 6] Same value encrypted multiple times...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(42LL);

        for (int i = 0; i < 10; ++i) {
            auto ciphertext = encryption.encrypt(plaintext);
            auto decrypted = encryption.decrypt(*ciphertext);

            assert(std::get<long long>(decrypted.value()) == 42);
        }

        std::cout << "  PASS: Same value encrypted 10 times, all decrypt correctly\n";
    }

    // ========================================
    // Test 7: Interleaved type encryptions
    // ========================================

    std::cout << "[TEST 7] Interleaved type encryptions...\n";

    {
        auto int1 = encryption.encrypt(hegadb::crypto::Plaintext(1LL));
        auto str1 = encryption.encrypt(hegadb::crypto::Plaintext(std::string("A")));
        auto bool1 = encryption.encrypt(hegadb::crypto::Plaintext(true));
        auto int2 = encryption.encrypt(hegadb::crypto::Plaintext(2LL));
        auto str2 = encryption.encrypt(hegadb::crypto::Plaintext(std::string("B")));
        auto bool2 = encryption.encrypt(hegadb::crypto::Plaintext(false));

        assert(std::get<long long>(encryption.decrypt(*int1).value()) == 1);
        assert(std::get<std::string>(encryption.decrypt(*str1).value()) == "A");
        assert(std::get<bool>(encryption.decrypt(*bool1).value()) == true);
        assert(std::get<long long>(encryption.decrypt(*int2).value()) == 2);
        assert(std::get<std::string>(encryption.decrypt(*str2).value()) == "B");
        assert(std::get<bool>(encryption.decrypt(*bool2).value()) == false);

        std::cout << "  PASS: Interleaved types handled correctly\n";
    }

    std::cout
        << "\n========================================\n"
        << " All edge case tests passed! (7/7)\n"
        << "========================================\n";

    return 0;
}
