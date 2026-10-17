#include "hegadb/crypto/KeyManager.h"
#include "cryptocontext-ser.h"
#include "scheme/bfvrns/bfvrns-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"
#include "key/key-ser.h"   // Registers EvalKeyRelinImpl<DCRTPoly> with Cereal — required for SerializeEvalMultKey
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace hegadb::crypto {

KeyManager::KeyManager(OpenFHEContext& context)
    : context_(context)
    , keysGenerated_(false)
    , bfvPublicKeyAvailable_(false)
    , ckksPublicKeyAvailable_(false)
{
}

void KeyManager::generateKeys()
{
    // Generate BFV key pair
    auto bfvContext = context_.getBFVContext();
    auto bfvKeyPair = bfvContext->KeyGen();
    bfvPublicKey_ = bfvKeyPair.publicKey;
    bfvPrivateKey_ = bfvKeyPair.secretKey;

    // Generate CKKS key pair
    auto ckksContext = context_.getCKKSContext();
    auto ckksKeyPair = ckksContext->KeyGen();
    ckksPublicKey_ = ckksKeyPair.publicKey;
    ckksPrivateKey_ = ckksKeyPair.secretKey;

    keysGenerated_ = true;
    bfvPublicKeyAvailable_ = true;
    ckksPublicKeyAvailable_ = true;
}

void KeyManager::generateEvaluationKeys()
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys must be generated before evaluation keys");
    }

    auto bfvContext = context_.getBFVContext();
    auto ckksContext = context_.getCKKSContext();

    // Generate multiplication/relinearization keys for BFV
    bfvContext->EvalMultKeyGen(bfvPrivateKey_);

    // Generate rotation keys for BFV (needed for string operations)
    // Generate keys for all possible rotations up to half the ring dimension
    std::vector<int32_t> indexList;
    int32_t ringDim = bfvContext->GetRingDimension();
    for (int32_t i = 1; i < static_cast<int32_t>(ringDim / 2); i *= 2) {
        indexList.push_back(i);
        indexList.push_back(-i);
    }
    bfvContext->EvalRotateKeyGen(bfvPrivateKey_, indexList);

    // Generate multiplication/relinearization keys for CKKS
    ckksContext->EvalMultKeyGen(ckksPrivateKey_);

    // Generate rotation keys for CKKS
    ckksContext->EvalRotateKeyGen(ckksPrivateKey_, indexList);
}

lbcrypto::PublicKey<lbcrypto::DCRTPoly> KeyManager::getBFVPublicKey() const
{
    if (!bfvPublicKeyAvailable_) {
        throw std::runtime_error("BFV public key is not available");
    }
    return bfvPublicKey_;
}

lbcrypto::PrivateKey<lbcrypto::DCRTPoly> KeyManager::getBFVPrivateKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return bfvPrivateKey_;
}

lbcrypto::PublicKey<lbcrypto::DCRTPoly> KeyManager::getCKKSPublicKey() const
{
    if (!ckksPublicKeyAvailable_) {
        throw std::runtime_error("CKKS public key is not available");
    }
    return ckksPublicKey_;
}

lbcrypto::PrivateKey<lbcrypto::DCRTPoly> KeyManager::getCKKSPrivateKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return ckksPrivateKey_;
}

bool KeyManager::hasKeys() const
{
    return keysGenerated_;
}

std::string KeyManager::serializeBFVPublicKey() const
{
    if (!bfvPublicKeyAvailable_) {
        throw std::runtime_error("BFV public key is not available");
    }
    
    std::ostringstream os;
    lbcrypto::Serial::Serialize(bfvPublicKey_, os, lbcrypto::SerType::BINARY);
    return os.str();
}

std::string KeyManager::serializeCKKSPublicKey() const
{
    if (!ckksPublicKeyAvailable_) {
        throw std::runtime_error("CKKS public key is not available");
    }
    
    std::ostringstream os;
    lbcrypto::Serial::Serialize(ckksPublicKey_, os, lbcrypto::SerType::BINARY);
    return os.str();
}

void KeyManager::deserializeBFVPublicKey(std::string const& serialized)
{
    std::istringstream is(serialized);
    lbcrypto::Serial::Deserialize(bfvPublicKey_, is, lbcrypto::SerType::BINARY);
    // Mark BFV public key as available after successful deserialization
    bfvPublicKeyAvailable_ = true;
}

void KeyManager::deserializeCKKSPublicKey(std::string const& serialized)
{
    std::istringstream is(serialized);
    lbcrypto::Serial::Deserialize(ckksPublicKey_, is, lbcrypto::SerType::BINARY);
    // Mark CKKS public key as available after successful deserialization
    ckksPublicKeyAvailable_ = true;
}

void KeyManager::saveKeys(std::string const& directory_path) const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Cannot save keys: keys have not been generated");
    }
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(directory_path);
    
    std::cout << "Saving keys to: " << directory_path << "\n";
    
    // First save the crypto contexts
    saveContext(directory_path);
    
    // Save BFV keys
    {
        std::ofstream bfv_private_file(directory_path + "/bfv_private_key.bin", std::ios::binary);
        if (!bfv_private_file.is_open()) {
            throw std::runtime_error("Cannot create BFV private key file");
        }
        lbcrypto::Serial::Serialize(bfvPrivateKey_, bfv_private_file, lbcrypto::SerType::BINARY);
        
        std::ofstream bfv_public_file(directory_path + "/bfv_public_key.bin", std::ios::binary);
        if (!bfv_public_file.is_open()) {
            throw std::runtime_error("Cannot create BFV public key file");
        }
        lbcrypto::Serial::Serialize(bfvPublicKey_, bfv_public_file, lbcrypto::SerType::BINARY);
    }
    
    // Save CKKS keys
    {
        std::ofstream ckks_private_file(directory_path + "/ckks_private_key.bin", std::ios::binary);
        if (!ckks_private_file.is_open()) {
            throw std::runtime_error("Cannot create CKKS private key file");
        }
        lbcrypto::Serial::Serialize(ckksPrivateKey_, ckks_private_file, lbcrypto::SerType::BINARY);
        
        std::ofstream ckks_public_file(directory_path + "/ckks_public_key.bin", std::ios::binary);
        if (!ckks_public_file.is_open()) {
            throw std::runtime_error("Cannot create CKKS public key file");
        }
        lbcrypto::Serial::Serialize(ckksPublicKey_, ckks_public_file, lbcrypto::SerType::BINARY);
    }

    // Save evaluation (relinearization) keys — required for EvalMult (AND / OR)
    {
        auto bfv_ctx  = context_.getBFVContext();
        auto ckks_ctx = context_.getCKKSContext();

        std::string bfv_eval_file_path = directory_path + "/bfv_eval_mult_key.bin";
        std::ofstream bfv_eval_file(bfv_eval_file_path, std::ios::binary);
        if (!bfv_eval_file.is_open()) {
            throw std::runtime_error("Cannot create BFV eval mult key file");
        }
        bool bfv_ok = bfv_ctx->SerializeEvalMultKey(bfv_eval_file, lbcrypto::SerType::BINARY, bfv_ctx);
        bfv_eval_file.close();
        if (!bfv_ok) {
            std::filesystem::remove(bfv_eval_file_path);
        }

        // CKKS eval mult keys are optional — only present if CKKS was used with EvalMult
        try {
            std::string ckks_eval_file_path = directory_path + "/ckks_eval_mult_key.bin";
            std::ofstream ckks_eval_file(ckks_eval_file_path, std::ios::binary);
            if (ckks_eval_file.is_open()) {
                bool ckks_ok = ckks_ctx->SerializeEvalMultKey(ckks_eval_file, lbcrypto::SerType::BINARY, ckks_ctx);
                ckks_eval_file.close();
                if (!ckks_ok) {
                    std::filesystem::remove(ckks_eval_file_path);
                }
            }
        } catch (...) {
            // CKKS eval mult keys not available — skip silently
        }
    }

    std::cout << "Keys saved successfully\n";
}

void KeyManager::loadKeys(std::string const& directory_path)
{
    std::cout << "Loading keys from: " << directory_path << "\n";
    
    if (!keyFilesExist(directory_path)) {
        throw std::runtime_error("Key files do not exist in directory: " + directory_path);
    }
    
    // Load crypto contexts (currently skipped - see implementation)
    loadContext(directory_path);
    
    // Load BFV keys
    {
        std::ifstream bfv_private_file(directory_path + "/bfv_private_key.bin", std::ios::binary);
        if (!bfv_private_file.is_open()) {
            throw std::runtime_error("Cannot open BFV private key file");
        }
        lbcrypto::Serial::Deserialize(bfvPrivateKey_, bfv_private_file, lbcrypto::SerType::BINARY);
        std::cout << "  BFV private key loaded\n";
        
        std::ifstream bfv_public_file(directory_path + "/bfv_public_key.bin", std::ios::binary);
        if (!bfv_public_file.is_open()) {
            throw std::runtime_error("Cannot open BFV public key file");
        }
        lbcrypto::Serial::Deserialize(bfvPublicKey_, bfv_public_file, lbcrypto::SerType::BINARY);
        std::cout << "  BFV public key loaded\n";
    }
    
    // Load CKKS keys
    {
        std::ifstream ckks_private_file(directory_path + "/ckks_private_key.bin", std::ios::binary);
        if (!ckks_private_file.is_open()) {
            throw std::runtime_error("Cannot open CKKS private key file");
        }
        lbcrypto::Serial::Deserialize(ckksPrivateKey_, ckks_private_file, lbcrypto::SerType::BINARY);
        std::cout << "  CKKS private key loaded\n";
        
        std::ifstream ckks_public_file(directory_path + "/ckks_public_key.bin", std::ios::binary);
        if (!ckks_public_file.is_open()) {
            throw std::runtime_error("Cannot open CKKS public key file");
        }
        lbcrypto::Serial::Deserialize(ckksPublicKey_, ckks_public_file, lbcrypto::SerType::BINARY);
        std::cout << "  CKKS public key loaded\n";
    }

    // Load evaluation (relinearization) keys if they were saved
    {
        auto bfv_eval_path  = directory_path + "/bfv_eval_mult_key.bin";
        auto ckks_eval_path = directory_path + "/ckks_eval_mult_key.bin";

        // Clear existing eval mult keys cache first so we don't conflict with stale keys
        context_.getBFVContext()->ClearEvalMultKeys();

        if (std::filesystem::exists(bfv_eval_path) && std::filesystem::file_size(bfv_eval_path) > 0) {
            try {
                std::ifstream bfv_eval_file(bfv_eval_path, std::ios::binary);
                context_.getBFVContext()->DeserializeEvalMultKey(bfv_eval_file,
                                                                 lbcrypto::SerType::BINARY);
                std::cout << "  BFV eval mult key loaded\n";
            } catch (std::exception const& e) {
                std::cerr << "  Warning: could not load BFV eval mult key: " << e.what() << "\n";
            }
        }

        if (std::filesystem::exists(ckks_eval_path) && std::filesystem::file_size(ckks_eval_path) > 0) {
            try {
                std::ifstream ckks_eval_file(ckks_eval_path, std::ios::binary);
                context_.getCKKSContext()->DeserializeEvalMultKey(ckks_eval_file,
                                                                  lbcrypto::SerType::BINARY);
                std::cout << "  CKKS eval mult key loaded\n";
            } catch (std::exception const& e) {
                std::cerr << "  Warning: could not load CKKS eval mult key: " << e.what() << "\n";
            }
        }
    }
    
    // Mark keys as available
    keysGenerated_ = true;
    bfvPublicKeyAvailable_ = true;
    ckksPublicKeyAvailable_ = true;
    
    std::cout << "Keys loaded successfully\n";
}

bool KeyManager::keyFilesExist(std::string const& directory_path) const
{
    return std::filesystem::exists(directory_path + "/bfv_private_key.bin") &&
           std::filesystem::exists(directory_path + "/bfv_public_key.bin") &&
           std::filesystem::exists(directory_path + "/ckks_private_key.bin") &&
           std::filesystem::exists(directory_path + "/ckks_public_key.bin") &&
           std::filesystem::exists(directory_path + "/bfv_context.bin") &&
           std::filesystem::exists(directory_path + "/ckks_context.bin");
}

void KeyManager::saveContext(std::string const& directory_path) const
{
    std::cout << "Saving crypto contexts...\n";
    
    // Save BFV context
    {
        std::ofstream bfv_context_file(directory_path + "/bfv_context.bin", std::ios::binary);
        if (!bfv_context_file.is_open()) {
            throw std::runtime_error("Cannot create BFV context file");
        }
        lbcrypto::Serial::Serialize(context_.getBFVContext(), bfv_context_file, lbcrypto::SerType::BINARY);
    }
    
    // Save CKKS context
    {
        std::ofstream ckks_context_file(directory_path + "/ckks_context.bin", std::ios::binary);
        if (!ckks_context_file.is_open()) {
            throw std::runtime_error("Cannot create CKKS context file");
        }
        lbcrypto::Serial::Serialize(context_.getCKKSContext(), ckks_context_file, lbcrypto::SerType::BINARY);
    }
    
    std::cout << "Crypto contexts saved successfully\n";
}

void KeyManager::loadContext(std::string const& directory_path)
{
    std::cout << "Loading crypto contexts...\n";
    
    // Check if context files exist
    if (!std::filesystem::exists(directory_path + "/bfv_context.bin") ||
        !std::filesystem::exists(directory_path + "/ckks_context.bin")) {
        throw std::runtime_error("Context files do not exist");
    }
    
    // Deserialize BFV context
    {
        std::ifstream bfv_context_file(directory_path + "/bfv_context.bin", std::ios::binary);
        if (!bfv_context_file.is_open()) {
            throw std::runtime_error("Cannot open BFV context file");
        }
        
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> loaded_bfv_context;
        lbcrypto::Serial::Deserialize(loaded_bfv_context, bfv_context_file, lbcrypto::SerType::BINARY);
        
        if (!loaded_bfv_context) {
            throw std::runtime_error("Failed to deserialize BFV context");
        }

        if (context_.getBFVContext() && context_.getBFVContext()->GetCryptoParameters()) {
            auto loaded_modulus = loaded_bfv_context->GetCryptoParameters()->GetPlaintextModulus();
            auto expected_modulus = context_.getBFVContext()->GetCryptoParameters()->GetPlaintextModulus();
            if (loaded_modulus != expected_modulus) {
                throw std::runtime_error("Stored BFV context has plaintext modulus " + std::to_string(loaded_modulus) +
                                         " but current configuration requires " + std::to_string(expected_modulus) +
                                         ". Stored keys are incompatible.");
            }
        }
        
        // Enable features on the loaded context (serialization doesn't preserve enabled features)
        loaded_bfv_context->Enable(lbcrypto::PKE);
        loaded_bfv_context->Enable(lbcrypto::KEYSWITCH);
        loaded_bfv_context->Enable(lbcrypto::LEVELEDSHE);
        loaded_bfv_context->Enable(lbcrypto::ADVANCEDSHE);
        
        // Replace the context's BFV context with the loaded one
        context_.setBFVContext(loaded_bfv_context);
        std::cout << "  BFV context loaded and replaced\n";
    }
    
    // Deserialize CKKS context
    {
        std::ifstream ckks_context_file(directory_path + "/ckks_context.bin", std::ios::binary);
        if (!ckks_context_file.is_open()) {
            throw std::runtime_error("Cannot open CKKS context file");
        }
        
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> loaded_ckks_context;
        lbcrypto::Serial::Deserialize(loaded_ckks_context, ckks_context_file, lbcrypto::SerType::BINARY);
        
        if (!loaded_ckks_context) {
            throw std::runtime_error("Failed to deserialize CKKS context");
        }
        
        // Enable features on the loaded context
        loaded_ckks_context->Enable(lbcrypto::PKE);
        loaded_ckks_context->Enable(lbcrypto::KEYSWITCH);
        loaded_ckks_context->Enable(lbcrypto::LEVELEDSHE);
        loaded_ckks_context->Enable(lbcrypto::ADVANCEDSHE);
        
        // Replace the context's CKKS context with the loaded one
        context_.setCKKSContext(loaded_ckks_context);
        std::cout << "  CKKS context loaded and replaced\n";
    }
    
    std::cout << "Crypto contexts loaded successfully\n";
}

} // namespace hegadb::crypto
