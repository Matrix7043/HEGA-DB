#include<hegadb/crypto/CompositeEncryption.h>
#include<hegadb/crypto/Plaintext.h>

#include<cassert>
#include<iostream>
#include<string>

int main(){
    std::cout
    << "========================================\n"
    << " HEGADB Crypto Basic Tests\n"
    << "========================================\n\n";

    std::cout << "Initializing OpenFHE encryption...\n";
    std::cout << "(This may take 2-5 seconds for key generation)\n\n";

    hegadb::crypto::CompositeEncryption encryption;
    std::cout << "Encryption initialized successfully.\n\n";


    // ========================================
    // Test 1: Integer encryption/decryption
    // ========================================

    std::cout << "[TEST 1] Integer encryption/decryption...\n";
    {
        auto plaintext = hegadb::crypto::Plaintext(42LL);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(decrypted.type() == hegadb::types::DataType::Integer);
        assert(std::get<long long>(decrypted.value()) == 42);

        std::cout<<"  PASS: 42 -> encrypted -> 42\n";
    }

    // ========================================
    // Test 2: Negative integer
    // ========================================

    std::cout << "[TEST 2] Negative integer...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(-100LL);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<long long>(decrypted.value()) == -100);
        std::cout << "  PASS: -100 -> encrypted -> -100\n";
    }
    

    // ========================================
    // Test 3: Zero
    // ========================================

    std::cout << "[TEST 3] Zero value...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(0LL);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<long long>(decrypted.value()) == 0);

        std::cout << "  PASS: 0 -> encrypted -> 0\n";
    }

    // ========================================
    // Test 4: Large integer
    // ========================================

    std::cout << "[TEST 4] Large integer...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(999999999LL);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<long long>(decrypted.value()) == 999999999LL);

        std::cout << "  PASS: 999999999 -> encrypted -> 999999999\n";
    }

    // ========================================
    // Test 5: String encryption
    // ========================================

    std::cout << "[TEST 5] String encryption...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(std::string("Hello"));
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(decrypted.type() == hegadb::types::DataType::String);
        assert(std::get<std::string>(decrypted.value()) == "Hello");

        std::cout << "  PASS: \"Hello\" -> encrypted -> \"Hello\"\n";
    }


    // ========================================
    // Test 6: Empty string
    // ========================================

    std::cout << "[TEST 6] Empty string...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(std::string(""));
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<std::string>(decrypted.value()) == "");

        std::cout << "  PASS: \"\" -> encrypted -> \"\"\n";
    }


    // ========================================
    // Test 7: String with spaces
    // ========================================

    std::cout << "[TEST 7] String with spaces...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(std::string("Hello World"));
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<std::string>(decrypted.value()) == "Hello World");

        std::cout << "  PASS: \"Hello World\" -> encrypted -> \"Hello World\"\n";
    }

    // ========================================
    // Test 8: Boolean true
    // ========================================

    std::cout << "[TEST 8] Boolean true...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(true);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(decrypted.type() == hegadb::types::DataType::Boolean);
        assert(std::get<bool>(decrypted.value()) == true);

        std::cout << "  PASS: true -> encrypted -> true\n";
    }

    // ========================================
    // Test 9: Boolean false
    // ========================================

    std::cout << "[TEST 9] Boolean false...\n";

    {
        auto plaintext = hegadb::crypto::Plaintext(false);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        assert(std::get<bool>(decrypted.value()) == false);

        std::cout << "  PASS: false -> encrypted -> false\n";
    }


    // ========================================
    // Test 10: Multiple independent encryptions
    // ========================================

    std::cout << "[TEST 10] Multiple independent encryptions...\n";

    {
        auto plain1 = hegadb::crypto::Plaintext(10LL);
        auto plain2 = hegadb::crypto::Plaintext(20LL);
        auto plain3 = hegadb::crypto::Plaintext(30LL);

        auto cipher1 = encryption.encrypt(plain1);
        auto cipher2 = encryption.encrypt(plain2);
        auto cipher3 = encryption.encrypt(plain3);

        auto decrypt1 = encryption.decrypt(*cipher1);
        auto decrypt2 = encryption.decrypt(*cipher2);
        auto decrypt3 = encryption.decrypt(*cipher3);

        assert(std::get<long long>(decrypt1.value()) == 10);
        assert(std::get<long long>(decrypt2.value()) == 20);
        assert(std::get<long long>(decrypt3.value()) == 30);

        std::cout << "  PASS: Multiple values encrypted independently\n";
    }

    std::cout
        << "\n========================================\n"
        << " All basic crypto tests passed! (10/10)\n"
        << "========================================\n";

    return 0;

}