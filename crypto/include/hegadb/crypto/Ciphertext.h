#ifndef CIPHERTEXT_H
#define CIPHERTEXT_H

#include<string>
#include<stdexcept>

namespace hegadb::crypto {

class Ciphertext {
public:
    virtual ~Ciphertext() = default;
    virtual std::string serialize() const{
        throw std::runtime_error("This ciphertext does not support serialization");
    }
};

}

#endif // !CIPHERTEXT_H
