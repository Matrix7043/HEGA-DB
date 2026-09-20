#ifndef CIPHERTEXT_H
#define CIPHERTEXT_H

namespace hegadb::crypto {

class Ciphertext {
public:
    virtual ~Ciphertext() = default;
    // TODO: virtual std::string serialize() = 0
};

}

#endif // !CIPHERTEXT_H
