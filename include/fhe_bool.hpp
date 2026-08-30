#pragma once

#include "lwe-ciphertext-fwd.h"

class FheBool {
private:
  lbcrypto::LWECiphertext ciphertext;

public:
  FheBool(lbcrypto::LWECiphertext ciphertext);

  lbcrypto::LWECiphertext getCiphertext() const;
};
