#include "hegadb/crypto/KeyManager.h"
#include "cryptocontext-ser.h"
#include "scheme/bfvrns/bfvrns-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace hegadb::crypto {

KeyManager::KeyManager(OpenFHEContext const& context)
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
        
        // Enable features on the loaded context (serialization doesn't preserve enabled features)
        loaded_bfv_context->Enable(lbcrypto::PKE);
        loaded_bfv_context->Enable(lbcrypto::KEYSWITCH);
        loaded_bfv_context->Enable(lbcrypto::LEVELEDSHE);
        loaded_bfv_context->Enable(lbcrypto::ADVANCEDSHE);
        
        // Replace the context's BFV context with the loaded one
        const_cast<crypto::OpenFHEContext&>(context_).setBFVContext(loaded_bfv_context);
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
        const_cast<crypto::OpenFHEContext&>(context_).setCKKSContext(loaded_ckks_context);
        std::cout << "  CKKS context loaded and replaced\n";
    }
    
    std::cout << "Crypto contexts loaded successfully\n";
}

} // namespace hegadb::crypto
