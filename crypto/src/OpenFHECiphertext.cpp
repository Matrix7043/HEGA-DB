#include "hegadb/crypto/OpenFHECiphertext.h"

// headers for serialization
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "scheme/bfvrns/bfvrns-ser.h"


#include<cstddef>
#include<cstdint>
#include<cstring>
#include<exception>
#include<sstream>
#include<stdexcept>
#include<string>
#include<utility>

namespace hegadb::crypto {
    namespace {

        /*
            Wire layout (all integer little-endian):

            offset  size    field
            0       4       magic "HGCT" (Hegadb ciphertext)
            4       1       format version
            5       1       scheme type (0 -> BFV, 1 -> CKKS)
            6       1       original datatype (types::DataType, numeric value)
            7       8       string length (0 unless the data type is String)
            15      ...     OpenFHE binary serialization of the ciphertext
        */


        constexpr char MAGIC[4] = {'H', 'G', 'C', 'T'};
        constexpr std::uint8_t FORMAT_VERSION = 1;
        constexpr std::size_t HEADER_SIZE = 15;

        static_assert(static_cast<int>(SchemeType::BFV) == 0
            && static_cast<int>(SchemeType::CKKS) == 1,
            "SchemeType values are part of the wire format");


        static_assert(static_cast<int>(types::DataType::Integer) == 0
                &&    static_cast<int>(types::DataType::Float) == 1
                &&    static_cast<int>(types::DataType::String) == 2
                &&    static_cast<int>(types::DataType::Boolean) == 3,
                "Datatype values are part of wire format");
    }// namespace

    OpenFHECiphertext::OpenFHECiphertext(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ciphertext,
        SchemeType schemeType,
        types::DataType originalType,
        size_t stringLength)
        : ciphertext_(ciphertext)
        , schemeType_(schemeType)
        , originalType_(originalType)
        , stringLength_(stringLength)
    {
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> OpenFHECiphertext::getCiphertext() const
    {
        return ciphertext_;
    }

    SchemeType OpenFHECiphertext::getSchemeType() const
    {
        return schemeType_;
    }

    types::DataType OpenFHECiphertext::getOriginalType() const
    {
        return originalType_;
    }

    size_t OpenFHECiphertext::getStringLength() const
    {
        return stringLength_;
    }

    std::string OpenFHECiphertext::serialize() const{
        if(!ciphertext_){
            throw std::runtime_error(
                "Cannot serialize an empty OpenFHE ciphertext"
            );
        }

        std::ostringstream blobStream;
        try{
            // returns void; failures are reported as exceptions.
            lbcrypto::Serial::Serialize(ciphertext_, blobStream, lbcrypto::SerType::BINARY);
        }catch(std::exception const& error){
            throw::std::runtime_error(
                std::string("OpenFHE ciphertext serialization failed")
                + error.what());
        }
        std::string const blob = blobStream.str();

        std::string bytes;
        bytes.reserve(HEADER_SIZE + blob.size());

        bytes.append(MAGIC, sizeof(MAGIC));
        bytes.push_back(static_cast<char>(FORMAT_VERSION));
        bytes.push_back(static_cast<char>(schemeType_));
        bytes.push_back(static_cast<char>(originalType_));

        auto const length = static_cast<std::uint64_t>(stringLength_);
        for(int i = 0; i<8; i++){
            bytes.push_back(static_cast<char>((length >> (8*i)) & 0xFFu));
        }
        bytes.append(blob);
        return bytes;
    }

    OpenFHECiphertext OpenFHECiphertext::deserialize(std::string const& bytes){
        // These bytes come from the network: validate everything before use.
        if(bytes.size() <= HEADER_SIZE){
            throw std::runtime_error("Malformed ciphertext, buffer too short");
        }

        if(std::memcmp(bytes.data(), MAGIC, sizeof(MAGIC)) != 0){
            throw std::runtime_error("Malformed ciphertext: bad magic (not a HEGADB ciphertext)");
        }

        auto const at = [&bytes](std::size_t index){
            return static_cast<std::uint8_t>(bytes[index]);
        };

        if(at(4) != FORMAT_VERSION){
            throw std::runtime_error(
                "Unsupported ciphertext format version "
                + std::to_string(static_cast<unsigned>(at(4))));
        }

        if(at(5) > static_cast<std::uint8_t>(SchemeType::CKKS)){
            throw std::runtime_error("Malformed ciphertext: unknown data type");
        }

        if(at(6) > static_cast<std::uint8_t>(types::DataType::Boolean)){
            throw std::runtime_error("Malformed ciphertext: unknown data type");
        }

        auto const scheme = static_cast<SchemeType>(at(5));
        auto const originalType = static_cast<types::DataType>(at(6));

        std::uint64_t stringLength = 0;
        for(int i = 0; i<8; i++){
            stringLength |= static_cast<std::uint64_t>(at(7 + i)) << (8*i);
        }

        // Reject metadata combinations the encryption engines never produce.
        // Decryption trusts this metadata, so check it up front.
        if (scheme == SchemeType::CKKS && originalType != types::DataType::Float) {
            throw std::runtime_error(
                "Malformed ciphertext: CKKS ciphertext must hold a Float");
        }

        if (scheme == SchemeType::BFV && originalType == types::DataType::Float) {
            throw std::runtime_error(
                "Malformed ciphertext: BFV ciphertext cannot hold a Float");
        }

        if (originalType != types::DataType::String && stringLength != 0) {
            throw std::runtime_error(
                "Malformed ciphertext: string length set on a non-string");
        }

        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ciphertext;

        try {
            std::istringstream blobStream(
                bytes.substr(HEADER_SIZE), std::ios::in | std::ios::binary);
    
            lbcrypto::Serial::Deserialize(
                ciphertext, blobStream, lbcrypto::SerType::BINARY);
        } catch (std::exception const& error) {
            throw std::runtime_error(
                std::string("OpenFHE ciphertext deserialization failed: ")
                + error.what());
        }
    
        if (!ciphertext) {
            throw std::runtime_error(
                "OpenFHE ciphertext deserialization produced an empty ciphertext");
        }
    
        return OpenFHECiphertext(
            std::move(ciphertext),
            scheme,
            originalType,
            static_cast<size_t>(stringLength));
    }


} // namespace hegadb::crypto
