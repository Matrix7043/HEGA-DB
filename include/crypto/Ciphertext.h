#ifndef CIPHERTEXT_H
#define CIPHERTEXT_H

#include <string>

namespace hegadb {

class Ciphertext {
public:
    explicit Ciphertext(std::string serialized);
    std::string const& serialized() const;

private:
    std::string serialized_;
};

}

#endif // !CIPHERTEXT_H
