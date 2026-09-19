#include "hegadb/crypto/OpenFHECiphertext.h"

namespace hegadb::crypto {

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

} // namespace hegadb::crypto
