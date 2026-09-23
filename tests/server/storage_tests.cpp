#include <gtest/gtest.h>
#include "hegadb/server/storage/storage_format.h"
#include "dictionary_encoder.h"
#include "hegadb/server/storage/pinned_memory_pool.h"
#include "mock_student_ingestion.h"
#include "hegadb/server/storage/segment_writer.h"
#include "hegadb/server/storage/segment_reader.h"
#include <hegadb/crypto/MockEncryption.h>
#include <hegadb/crypto/MockCiphertext.h>
#include <hegadb/crypto/Plaintext.h>
#include <filesystem>
#include <vector>

#include <iostream>
#include <iomanip>

using namespace hegadb::server::storage;

TEST(StorageFormatTest, AssertSizes) {
    EXPECT_EQ(sizeof(SegmentFileHeader), 64);
    EXPECT_EQ(sizeof(BlockHeader), 64);
}

TEST(DictionaryEncoderTest, EncodeDecode) {
    DictionaryEncoder encoder;
    
    EXPECT_EQ(encoder.encode("Computer Science"), 1);
    EXPECT_EQ(encoder.encode("Electrical Eng"), 2);
    EXPECT_EQ(encoder.encode("Computer Science"), 1); // Should reuse ID
    
    auto decoded1 = encoder.decode(1);
    ASSERT_TRUE(decoded1.has_value());
    EXPECT_EQ(decoded1.value, "Computer Science");
    
    auto decoded0 = encoder.decode(0);
    EXPECT_FALSE(decoded0.has_value());
    EXPECT_EQ(decoded0.error, StorageError::InvalidDictionaryId);
}

TEST(PinnedMemoryPoolTest, BufferSwap) {
    PinnedMemoryPool pool(1024);
    auto buf1 = pool.get_primary_buffer();
    auto buf2 = pool.get_secondary_buffer();
    EXPECT_EQ(buf1.size(), 1024);
    EXPECT_EQ(buf2.size(), 1024);
    
    pool.swap_buffers();
    // After swap, primary should point to what was secondary
    EXPECT_EQ(pool.get_primary_buffer().data(), buf2.data());
    EXPECT_EQ(pool.get_secondary_buffer().data(), buf1.data());
}

TEST(SegmentWriterReaderTest, RoundTrip) {
    std::string test_file = "test_segment.bin";
    
    // 1. Write
    SegmentFileHeader header = {
        {'T','E','S','T','_','S','E','G'}, 1, 1, 1024, 1, 100, 0, 12345, 1, 100, {0}
    };
    
    {
        SegmentWriter writer(test_file, header);
        BlockHeader block = {0, 50, 100, 256, 0, {0}};
        std::vector<std::byte> payload(256, std::byte{0xAA});
        auto res = writer.write_block(block, payload);
        ASSERT_TRUE(res.success);
        writer.close();
    }
    
    // 2. Read
    {
        SegmentReader reader(test_file);
        auto read_header = reader.read_header();
        ASSERT_TRUE(read_header.has_value());
        
        EXPECT_EQ(read_header->version, 1);
        EXPECT_EQ(read_header->poly_degree_N, 1024);
        
        std::vector<std::byte> read_payload;
        auto read_block = reader.read_next_block(read_payload);
        ASSERT_TRUE(read_block.has_value());
        
        EXPECT_EQ(read_block->active_rows, 50);
        EXPECT_EQ(read_block->current_noise_budget, 100);
        EXPECT_EQ(read_payload.size(), 256);
        for (auto b : read_payload) {
            EXPECT_EQ(b, std::byte{0xAA});
        }
    }
    
    std::filesystem::remove(test_file);
}

TEST(StorageEngineTest, EncryptedRowRoundTrip) {
    std::string test_file = "test_encrypted_segment.bin";

    std::cout << "\n================ STORAGE ENGINE ENCRYPTED ROW TEST ================\n";

    // 1. Simulate Client Encryption
    hegadb::crypto::MockEncryption crypto_mock;
    hegadb::crypto::Plaintext pt_value(42LL); // 42 as long long for types::DataType::Integer
    std::cout << "[Client] Encrypting Plaintext value: 42\n";

    auto ciphertext = crypto_mock.encrypt(pt_value);
    std::string serialized_bytes = ciphertext->serialize();

    std::cout << "[Client] Serialized Ciphertext Payload Size: " << serialized_bytes.size() << " bytes\n";
    std::cout << "[Client] Encrypted Payload (raw text view): " << serialized_bytes << "\n";
    std::cout << "[Client] Encrypted Payload (hex view): ";
    for (unsigned char c : serialized_bytes) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << "\n\n";

    // 2. Server Storage Ingestion
    std::cout << "[Server] Writing encrypted block payload to segment file: " << test_file << "...\n";
    SegmentFileHeader header = {
        {'M','O','C','K','_','S','E','G'}, 1, 1, 1024, 1, 100, 0, 12345, 1, 1, {0}
    };
    
    {
        SegmentWriter writer(test_file, header);
        BlockHeader block = {0, 1, 100, static_cast<uint32_t>(serialized_bytes.size()), 0, {0}};
        
        // Convert string bytes to span<const std::byte>
        std::span<const std::byte> payload_span(
            reinterpret_cast<const std::byte*>(serialized_bytes.data()), 
            serialized_bytes.size()
        );
        
        auto res = writer.write_block(block, payload_span);
        ASSERT_TRUE(res.success);
        writer.close();
    }
    std::cout << "[Server] Block successfully written to disk.\n\n";
    
    // 3. Server Storage Retrieval
    std::cout << "[Server] Reading encrypted block payload back from segment file...\n";
    std::vector<std::byte> read_payload;
    {
        SegmentReader reader(test_file);
        auto read_header = reader.read_header();
        ASSERT_TRUE(read_header.has_value());
        
        auto read_block = reader.read_next_block(read_payload);
        ASSERT_TRUE(read_block.has_value());
        
        EXPECT_EQ(read_payload.size(), serialized_bytes.size());
    }

    // 4. Verification
    std::string retrieved_bytes(reinterpret_cast<const char*>(read_payload.data()), read_payload.size());
    std::cout << "[Server] Retrieved Payload Size: " << retrieved_bytes.size() << " bytes\n";
    std::cout << "[Server] Retrieved Encrypted Payload (hex view): ";
    for (unsigned char c : retrieved_bytes) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << "\n";

    EXPECT_EQ(retrieved_bytes, serialized_bytes);
    std::cout << "[Test Result] SUCCESS: Retrieved encrypted payload matches original client ciphertext exactly!\n";
    std::cout << "===================================================================\n\n";

    std::filesystem::remove(test_file);
}
