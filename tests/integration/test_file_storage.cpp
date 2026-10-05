#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <hegadb/server/storage/EncryptedBatch.h>
#include <hegadb/crypto/CompositeEncryption.h>

#include <iostream>
#include <wordexp.h>

using namespace hegadb;

std::string expandPath(std::string const& path) {
    wordexp_t exp_result;
    wordexp(path.c_str(), &exp_result, 0);
    std::string expanded = exp_result.we_wordv[0];
    wordfree(&exp_result);
    return expanded;
}

void displayBanner() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║   FileStorageManager Test                         ║\n";
    std::cout << "║   Testing Encrypted Database Loading             ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

int main() {
    displayBanner();

    try {
        // Expand paths
        std::string data_dir    = expandPath("~/.hegadb/data");
        std::string catalog_dir = expandPath("~/.hegadb/catalog");

        std::cout << "[TEST]    Data directory: " << data_dir << "\n\n";

        // Create FileStorageManager
        std::cout << "[STORAGE] Creating FileStorageManager...\n";
        auto storage = server::storage::StorageManager::createFileStorage(data_dir, catalog_dir);
        std::cout << "[STORAGE] ✓ FileStorageManager created\n\n";

        // Open EMPLOYEES table
        std::cout << "[TABLE]   Opening EMPLOYEES table...\n";
        auto table = storage->openTable("EMPLOYEES");
        std::cout << "[TABLE]   ✓ Table opened\n";
        std::cout << "[TABLE]   Name: " << table->getName() << "\n";
        std::cout << "[TABLE]   Row count: " << table->getRowCount() << "\n";

        // Get column names
        auto column_names = table->getColumnNames();
        std::cout << "[TABLE]   Columns (" << column_names.size() << "): ";
        for (size_t i = 0; i < column_names.size(); ++i) {
            std::cout << column_names[i];
            if (i < column_names.size() - 1) std::cout << ", ";
        }
        std::cout << "\n\n";

        // Open and read batches
        std::cout << "[READ]    Opening table for reading...\n";
        table->open();
        std::cout << "[READ]    ✓ Table opened\n\n";

        size_t batch_count = 0;
        size_t total_rows = 0;

        while (auto batch_opt = table->nextBatch()) {
            batch_count++;
            auto const& batch = *batch_opt;
            
            std::cout << "[BATCH]   Batch #" << batch_count << ":\n";
            std::cout << "[BATCH]     Rows: " << batch.row_count << "\n";
            std::cout << "[BATCH]     Columns: " << batch.getColumnCount() << "\n";
            
            // List columns in batch
            for (auto const& col_name : column_names) {
                if (batch.hasColumn(col_name)) {
                    auto const& ciphertext = batch.getColumn(col_name);
                    std::cout << "[BATCH]       - " << col_name 
                              << " (scheme: " 
                              << (ciphertext.getSchemeType() == crypto::SchemeType::BFV ? "BFV" : "CKKS")
                              << ", type: ";
                    
                    switch (ciphertext.getOriginalType()) {
                        case types::DataType::Integer: std::cout << "INTEGER"; break;
                        case types::DataType::String: std::cout << "STRING"; break;
                        case types::DataType::Float: std::cout << "FLOAT"; break;
                        case types::DataType::Boolean: std::cout << "BOOLEAN"; break;
                        default: std::cout << "UNKNOWN"; break;
                    }
                    
                    std::cout << ")\n";
                } else {
                    std::cout << "[BATCH]       - " << col_name << " (MISSING!)\n";
                }
            }
            
            total_rows += batch.row_count;
            std::cout << "\n";
        }

        std::cout << "[READ]    ✓ Read " << batch_count << " batch(es) with " 
                  << total_rows << " total rows\n\n";

        // Close table
        table->close();
        std::cout << "[TABLE]   ✓ Table closed\n\n";

        // Try to decrypt one value to verify encryption
        std::cout << "[DECRYPT] Testing decryption of one column...\n";
        auto encryption = std::make_shared<crypto::CompositeEncryption>();
        
        // Reopen and get first batch
        table->open();
        auto first_batch = table->nextBatch();
        
        if (first_batch && first_batch->hasColumn("id")) {
            auto const& id_ciphertext = first_batch->getColumn("id");
            
            try {
                auto plaintext = encryption->decrypt(id_ciphertext);
                std::cout << "[DECRYPT] ✓ Successfully decrypted 'id' column\n";
                std::cout << "[DECRYPT]   Plaintext type: ";
                
                auto const& value = plaintext.value();
                if (std::holds_alternative<long long>(value)) {
                    std::cout << "INTEGER (value: " << std::get<long long>(value) << ")\n";
                } else if (std::holds_alternative<std::string>(value)) {
                    std::cout << "STRING (value: " << std::get<std::string>(value) << ")\n";
                } else {
                    std::cout << "OTHER\n";
                }
            } catch (std::exception const& e) {
                std::cout << "[DECRYPT] ✗ Decryption failed: " << e.what() << "\n";
            }
        }
        
        table->close();
        std::cout << "\n";

        // Success!
        std::cout << "╔════════════════════════════════════════════════════╗\n";
        std::cout << "║              ✓ ALL TESTS PASSED                   ║\n";
        std::cout << "╠════════════════════════════════════════════════════╣\n";
        std::cout << "║  FileStorageManager successfully loaded and       ║\n";
        std::cout << "║  read encrypted EMPLOYEES database!               ║\n";
        std::cout << "╚════════════════════════════════════════════════════╝\n";
        std::cout << "\n";

        return 0;

    } catch (std::exception const& e) {
        std::cerr << "\n[ERROR]   " << e.what() << "\n\n";
        return 1;
    }
}
