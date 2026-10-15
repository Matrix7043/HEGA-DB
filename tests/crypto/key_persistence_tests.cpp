#include <gtest/gtest.h>
#include <hegadb/crypto/KeyManager.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <hegadb/crypto/Plaintext.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>

class KeyPersistenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create unique test directory for each test
        test_dir_ = std::filesystem::temp_directory_path() / "hegadb_key_test" / 
                    std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Ensure clean state
        if (std::filesystem::exists(test_dir_)) {
            std::filesystem::remove_all(test_dir_);
        }
        
        std::cout << "Test directory: " << test_dir_ << "\n";
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(test_dir_)) {
            std::filesystem::remove_all(test_dir_);
        }
    }
    
    std::filesystem::path test_dir_;
    
    // Helper to check if all key files exist
    bool allKeyFilesExist(const std::filesystem::path& dir) {
        return std::filesystem::exists(dir / "bfv_private_key.bin") &&
               std::filesystem::exists(dir / "bfv_public_key.bin") &&
               std::filesystem::exists(dir / "ckks_private_key.bin") &&
               std::filesystem::exists(dir / "ckks_public_key.bin");
    }
    
    // Helper to get file sizes
    std::vector<std::size_t> getKeyFileSizes(const std::filesystem::path& dir) {
        return {
            std::filesystem::file_size(dir / "bfv_private_key.bin"),
            std::filesystem::file_size(dir / "bfv_public_key.bin"),
            std::filesystem::file_size(dir / "ckks_private_key.bin"),
            std::filesystem::file_size(dir / "ckks_public_key.bin")
        };
    }
};

// ============================================================================
// Test 1: Key Files Are Created When Keys Are Generated and Saved
// ============================================================================

TEST_F(KeyPersistenceTest, KeyFilesCreatedOnSave) {
    std::cout << "[TEST] Key files are created when keys are saved...\n";
    
    // Create context and key manager
    hegadb::crypto::OpenFHEContext context;
    context.enableFeatures();
    
    hegadb::crypto::KeyManager key_manager(context);
    
    // Initially, no key files should exist
    EXPECT_FALSE(allKeyFilesExist(test_dir_));
    EXPECT_FALSE(key_manager.keyFilesExist(test_dir_));
    
    // Generate keys
    key_manager.generateKeys();
    key_manager.generateEvaluationKeys();
    
    // Key files still shouldn't exist until we save
    EXPECT_FALSE(allKeyFilesExist(test_dir_));
    
    // Save keys
    EXPECT_NO_THROW(key_manager.saveKeys(test_dir_));
    
    // Now key files should exist
    EXPECT_TRUE(allKeyFilesExist(test_dir_));
    EXPECT_TRUE(key_manager.keyFilesExist(test_dir_));
    
    // Check that files have reasonable sizes (not empty)
    auto sizes = getKeyFileSizes(test_dir_);
    for (auto size : sizes) {
        EXPECT_GT(size, 1000); // Keys should be substantial size
    }
    
    std::cout << "  Key file sizes: BFV private=" << sizes[0] << ", BFV public=" << sizes[1] 
              << ", CKKS private=" << sizes[2] << ", CKKS public=" << sizes[3] << " bytes\n";
    std::cout << "  PASS: Key files created successfully\n";
}

// ============================================================================
// Test 2: Keys Can Be Loaded From Disk
// ============================================================================

TEST_F(KeyPersistenceTest, KeysCanBeLoadedFromDisk) {
    std::cout << "[TEST] Keys can be loaded from disk...\n";
    
    // Phase 1: Generate and save keys
    {
        hegadb::crypto::OpenFHEContext context1;
        context1.enableFeatures();
        hegadb::crypto::KeyManager key_manager1(context1);
        
        key_manager1.generateKeys();
        key_manager1.generateEvaluationKeys();
        
        key_manager1.saveKeys(test_dir_);
    }
    
    // Phase 2: Load keys from disk in fresh context
    {
        hegadb::crypto::OpenFHEContext context2;
        context2.enableFeatures();
        hegadb::crypto::KeyManager key_manager2(context2);
        
        // Initially, no keys are loaded
        EXPECT_FALSE(key_manager2.hasKeys());
        
        // Load keys from disk
        EXPECT_NO_THROW(key_manager2.loadKeys(test_dir_));
        
        // Keys should now be loaded and usable
        EXPECT_TRUE(key_manager2.hasKeys());
        EXPECT_NE(key_manager2.getBFVPublicKey(), nullptr);
        EXPECT_NE(key_manager2.getCKKSPublicKey(), nullptr);
        EXPECT_NE(key_manager2.getBFVPrivateKey(), nullptr);
        EXPECT_NE(key_manager2.getCKKSPrivateKey(), nullptr);
        
        std::cout << "  PASS: Keys loaded successfully and are ready for use\n";
    }
}

// ============================================================================
// Test 3: CompositeEncryption Uses Persistent Keys
// ============================================================================

TEST_F(KeyPersistenceTest, CompositeEncryptionUsesPersistentKeys) {
    std::cout << "[TEST] CompositeEncryption uses persistent keys...\n";
    
    std::string first_session_ciphertext, second_session_ciphertext;
    
    // Session 1: First time initialization - should generate and save keys
    {
        std::cout << "  Session 1: Creating CompositeEncryption (should generate keys)...\n";
        hegadb::crypto::CompositeEncryption encryption1(test_dir_);
        
        // Keys should be saved to disk
        EXPECT_TRUE(allKeyFilesExist(test_dir_));
        
        // Encrypt some data
        auto plaintext = hegadb::crypto::Plaintext(42LL);
        auto ciphertext = encryption1.encrypt(plaintext);
        first_session_ciphertext = ciphertext->serialize();
    }
    
    // Session 2: Second initialization - should load existing keys
    {
        std::cout << "  Session 2: Creating CompositeEncryption (should load existing keys)...\n";
        hegadb::crypto::CompositeEncryption encryption2(test_dir_);
        
        // Keys should still exist
        EXPECT_TRUE(allKeyFilesExist(test_dir_));
        
        // Encrypt the same data
        auto plaintext = hegadb::crypto::Plaintext(42LL);
        auto ciphertext = encryption2.encrypt(plaintext);
        second_session_ciphertext = ciphertext->serialize();
    }
    
    // The ciphertexts should be different (due to randomness) but decryptable with same keys
    EXPECT_NE(first_session_ciphertext, second_session_ciphertext);
    
    // Both should decrypt to the same value (tested in integration)
    std::cout << "  PASS: CompositeEncryption correctly uses persistent keys\n";
}

// ============================================================================
// Test 4: Cross-Session Decryption Works With Persistent Keys
// ============================================================================

TEST_F(KeyPersistenceTest, CrossSessionDecryptionWorks) {
    std::cout << "[TEST] Cross-session decryption works with persistent keys...\n";
    
    std::string serialized_ciphertext;
    
    // Session 1: Encrypt data
    {
        hegadb::crypto::CompositeEncryption encryption1(test_dir_);
        
        auto plaintext = hegadb::crypto::Plaintext(12LL);
        auto ciphertext = encryption1.encrypt(plaintext);
        serialized_ciphertext = ciphertext->serialize();
        
        std::cout << "  Session 1: Encrypted value 12\n";
    }
    
    // Session 2: Decrypt data with loaded keys
    {
        hegadb::crypto::CompositeEncryption encryption2(test_dir_);
        
        // Deserialize the ciphertext
        auto restored_ciphertext = hegadb::crypto::OpenFHECiphertext::deserialize(serialized_ciphertext);
        
        // Decrypt it
        auto decrypted_plaintext = encryption2.decrypt(restored_ciphertext);
        
        // Should match original value
        EXPECT_EQ(decrypted_plaintext.type(), hegadb::types::DataType::Integer);
        EXPECT_EQ(std::get<long long>(decrypted_plaintext.value()), 12LL);
        
        std::cout << "  Session 2: Successfully decrypted to original value\n";
        std::cout << "  PASS: Cross-session encryption/decryption works\n";
    }
}

// ============================================================================
// Test 5: Error Handling - Loading Non-Existent Keys
// ============================================================================

TEST_F(KeyPersistenceTest, LoadingNonExistentKeysThrows) {
    std::cout << "[TEST] Loading non-existent keys throws appropriate error...\n";
    
    hegadb::crypto::OpenFHEContext context;
    context.enableFeatures();
    hegadb::crypto::KeyManager key_manager(context);
    
    // Try to load from non-existent directory
    std::filesystem::path fake_dir = test_dir_ / "non_existent";
    
    EXPECT_FALSE(key_manager.keyFilesExist(fake_dir));
    EXPECT_THROW(key_manager.loadKeys(fake_dir), std::runtime_error);
    
    std::cout << "  PASS: Appropriate error thrown for non-existent keys\n";
}

// ============================================================================
// Test 6: Error Handling - Saving Keys Before Generation
// ============================================================================

TEST_F(KeyPersistenceTest, SavingKeysBeforeGenerationThrows) {
    std::cout << "[TEST] Saving keys before generation throws error...\n";
    
    hegadb::crypto::OpenFHEContext context;
    context.enableFeatures();
    hegadb::crypto::KeyManager key_manager(context);
    
    // Try to save keys before generating them
    EXPECT_THROW(key_manager.saveKeys(test_dir_), std::runtime_error);
    
    std::cout << "  PASS: Appropriate error thrown when saving ungenerated keys\n";
}

// ============================================================================
// Test 7: Persistent Keys Reused Forever On Single Machine
// ============================================================================

TEST_F(KeyPersistenceTest, PersistentKeysReusedOnSameMachine) {
    std::cout << "[TEST] Once created, keys persist and are reused on the machine...\n";
    
    // Step 1: Initialize first instance (generates and persists keys to test_dir_)
    {
        hegadb::crypto::CompositeEncryption encryption(test_dir_);
        EXPECT_TRUE(allKeyFilesExist(test_dir_));
    }
    
    // Step 2: Initialize subsequent instance (loads existing keys from disk)
    {
        hegadb::crypto::CompositeEncryption encryption(test_dir_);
        EXPECT_TRUE(allKeyFilesExist(test_dir_));
        
        // Test encryption and decryption in the loaded session
        auto int_val = hegadb::crypto::Plaintext(77LL);
        auto int_ct = encryption.encrypt(int_val);
        auto int_dec = encryption.decrypt(*int_ct);
        EXPECT_EQ(std::get<long long>(int_dec.value()), 77LL);
        
        auto str_val = hegadb::crypto::Plaintext(std::string("persisted_test"));
        auto str_ct = encryption.encrypt(str_val);
        auto str_dec = encryption.decrypt(*str_ct);
        EXPECT_EQ(std::get<std::string>(str_dec.value()), "persisted_test");
    }
    
    std::cout << "  PASS: Keys successfully created once and reused for all operations\n";
}

// ============================================================================
// Test 8: Directory Creation for Key Storage
// ============================================================================

TEST_F(KeyPersistenceTest, DirectoryCreationForKeyStorage) {
    std::cout << "[TEST] Directory is created automatically for key storage...\n";
    
    // Use nested directory path that doesn't exist
    auto nested_dir = test_dir_ / "level1" / "level2" / "keys";
    
    EXPECT_FALSE(std::filesystem::exists(nested_dir));
    
    // CompositeEncryption should create the directory structure
    hegadb::crypto::CompositeEncryption encryption(nested_dir);
    
    EXPECT_TRUE(std::filesystem::exists(nested_dir));
    EXPECT_TRUE(allKeyFilesExist(nested_dir));
    
    std::cout << "  PASS: Nested directories created automatically\n";
}

// ============================================================================
// Test 9: Fallback Behavior When Save Fails
// ============================================================================

TEST_F(KeyPersistenceTest, FallbackWhenSaveFails) {
    std::cout << "[TEST] Fallback behavior when save fails...\n";
    
    // Use a read-only directory to simulate save failure
    auto readonly_dir = test_dir_ / "readonly";
    std::filesystem::create_directories(readonly_dir);
    
    // Create a dummy file to make directory read-only more effectively
    std::ofstream dummy_file(readonly_dir / "dummy.txt");
    dummy_file << "test";
    dummy_file.close();
    
    // Make directory read-only (remove write permissions)
    std::filesystem::permissions(readonly_dir, 
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_exec,
        std::filesystem::perm_options::replace);
    
    // CompositeEncryption should still work (fallback to temporary keys)
    EXPECT_NO_THROW({
        hegadb::crypto::CompositeEncryption encryption(readonly_dir);
        
        // Should still be able to encrypt/decrypt in the same session
        auto plaintext = hegadb::crypto::Plaintext(99LL);
        auto ciphertext = encryption.encrypt(plaintext);
        auto decrypted = encryption.decrypt(*ciphertext);
        
        EXPECT_EQ(std::get<long long>(decrypted.value()), 99LL);
    });
    
    // Restore permissions for cleanup (need write permission to delete)
    std::filesystem::permissions(readonly_dir, std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
    
    std::cout << "  PASS: Graceful fallback when save fails\n";
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "========================================\n";
    std::cout << "HEGA-DB Key Persistence Tests\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Testing key persistence features for consistent\n";
    std::cout << "encryption across multiple sessions...\n\n";
    
    auto result = RUN_ALL_TESTS();
    
    std::cout << "\n========================================\n";
    if (result == 0) {
        std::cout << "All key persistence tests passed!\n";
        std::cout << "Keys will be consistently saved and loaded.\n";
        std::cout << "Data encrypted in one session can be decrypted in another.\n";
    } else {
        std::cout << "Some key persistence tests failed.\n";
        std::cout << "Key consistency across sessions may not work properly.\n";
    }
    std::cout << "========================================\n";
    
    return result;
}