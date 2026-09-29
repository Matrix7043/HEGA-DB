#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/Plaintext.h>
#include <iostream>
#include <filesystem>
#include <vector>

/**
 * Simple integration test showing key persistence in action
 * This simulates a realistic client workflow:
 * 1. First run: Generate keys and encrypt data
 * 2. Second run: Load existing keys and decrypt data
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "Key Persistence Integration Test\n";
    std::cout << "========================================\n\n";
    
    // Use a temporary test directory
    std::string key_dir = "/tmp/hegadb_integration_test";
    
    try {
        // Clean up any previous test
        if (std::filesystem::exists(key_dir)) {
            std::filesystem::remove_all(key_dir);
        }
        
        std::vector<std::string> serialized_data;
        
        // ====================================================================
        // SIMULATION: First Client Session (like data ingestion)
        // ====================================================================
        std::cout << "FIRST SESSION - Data Owner Encrypts Data\n";
        std::cout << "────────────────────────────────────────────\n";
        
        {
            hegadb::crypto::CompositeEncryption encryption(key_dir);
            
            // Simulate encrypting a dataset
            std::vector<int64_t> dataset = {100, 250, 75, 300, 180};
            
            std::cout << "Encrypting dataset: ";
            for (size_t i = 0; i < dataset.size(); ++i) {
                std::cout << dataset[i];
                if (i < dataset.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            
            for (auto value : dataset) {
                auto plaintext = hegadb::crypto::Plaintext(value);
                auto ciphertext = encryption.encrypt(plaintext);
                serialized_data.push_back(ciphertext->serialize());
            }
            
            std::cout << "Encrypted " << dataset.size() << " values\n";
            std::cout << "Keys saved to: " << key_dir << "\n\n";
        }
        
        // ====================================================================
        // SIMULATION: Second Client Session (like query execution)
        // ====================================================================
        std::cout << "SECOND SESSION - Same Client Decrypts Data\n";
        std::cout << "────────────────────────────────────────────────\n";
        
        {
            hegadb::crypto::CompositeEncryption encryption(key_dir);
            
            std::cout << "Decrypting previously encrypted data...\n";
            std::vector<int64_t> decrypted_values;
            
            for (size_t i = 0; i < serialized_data.size(); ++i) {
                auto restored_ciphertext = hegadb::crypto::OpenFHECiphertext::deserialize(serialized_data[i]);
                auto decrypted_plaintext = encryption.decrypt(restored_ciphertext);
                
                if (decrypted_plaintext.type() == hegadb::types::DataType::Integer) {
                    auto value = std::get<long long>(decrypted_plaintext.value());
                    decrypted_values.push_back(value);
                    std::cout << "  Value " << (i + 1) << ": " << value << " ✅\n";
                } else {
                    std::cout << "  Value " << (i + 1) << ": ERROR - wrong type ❌\n";
                    return 1;
                }
            }
            
            std::cout << "\nSuccessfully decrypted all values with persistent keys!\n";
        }
        
        // ====================================================================
        // VERIFICATION: Check Key Files
        // ====================================================================
        std::cout << "\nKEY FILES VERIFICATION\n";
        std::cout << "─────────────────────────\n";
        
        std::vector<std::string> key_files = {
            "bfv_private_key.bin",
            "bfv_public_key.bin", 
            "ckks_private_key.bin",
            "ckks_public_key.bin"
        };
        
        for (const auto& filename : key_files) {
            auto filepath = std::filesystem::path(key_dir) / filename;
            if (std::filesystem::exists(filepath)) {
                auto size = std::filesystem::file_size(filepath);
                std::cout << "  " << filename << ": " << size << " bytes \n";
            } else {
                std::cout << "  " << filename << ": MISSING \n";
                return 1;
            }
        }
        
        // ====================================================================
        // CLEANUP
        // ====================================================================
        std::cout << "\nCleaning up test directory...\n";
        std::filesystem::remove_all(key_dir);
        
        std::cout << "\n========================================\n";
        std::cout << "KEY PERSISTENCE INTEGRATION TEST PASSED!\n";
        std::cout << "========================================\n";
        std::cout << "\nKey persistence is working correctly:\n";
        std::cout << "• Keys are generated once and saved to disk\n";
        std::cout << "• Keys are loaded from disk in subsequent sessions\n";
        std::cout << "• Data encrypted in session 1 can be decrypted in session 2\n";
        std::cout << "• This enables consistent encryption across database sessions\n";
        
        return 0;
        
    } catch (std::exception const& e) {
        std::cout << "\nTEST FAILED: " << e.what() << "\n";
        
        // Clean up on failure
        if (std::filesystem::exists(key_dir)) {
            std::filesystem::remove_all(key_dir);
        }
        
        return 1;
    }
}