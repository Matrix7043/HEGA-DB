#include<hegadb/crypto/CompositeEncryption.h>
#include<hegadb/crypto/Plaintext.h>

#include<cassert>
#include<iostream>
#include<string>
#include<vector>

struct StringTestCase{
    std::string name;
    std::string value;
};

int main(){
    std::cout
    << "========================================\n"
    << " HEGADB Crypto String Tests\n"
    << "========================================\n\n";

    std::cout << "Initializing encryption...\n\n";

    hegadb::crypto::CompositeEncryption encryption;

        std::vector<StringTestCase> const tests = {
        { "Single character", "A" },
        { "Short word", "cat" },
        { "Medium word", "Hello" },
        { "Long word", "Constantinople" },
        { "With numbers", "Test123" },
        { "With special chars", "Hello@World!" },
        { "With punctuation", "Hello, World." },
        { "SQL name", "Alice" },
        { "SQL name 2", "Bob" },
        { "Department", "Engineering" },
        { "Multiple spaces", "Hello   World" },
        { "Leading space", " Hello" },
        { "Trailing space", "Hello " },
        { "Tab character", "Hello\tWorld" },
        { "Newline character", "Hello\nWorld" },
        { "Single space", " " },
        { "Mixed case", "HeLLo WoRLd" },
        { "All uppercase", "HELLO WORLD" },
        { "All lowercase", "hello world" },
        { "Digits only", "1234567890" }
    };

    int passed = 0;

    for(auto const& test: tests){
        auto plaintext = hegadb::crypto::Plaintext(test.value);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);

        auto const& result = std::get<std::string>(decrypted.value());

        if(result == test.value){
            ++passed;
            std::cout
                << "[PASS] "
                << test.name
                << ": \""
                << test.value
                << "\"\n";
        } else {
            std::cout
                << "[FAIL] "
                << test.name
                << "\n"
                << "  Expected: \""
                << test.value
                << "\"\n"
                << "  Got: \""
                << result
                << "\"\n";
        }
    }

    std::cout
    << "\n========================================\n"
    << " Result: "
    << passed
    << "/"
    << tests.size()
    << " string tests passed\n"
    << "========================================\n";

    return passed == static_cast<int>(tests.size()) ? 0 : 1;
}
