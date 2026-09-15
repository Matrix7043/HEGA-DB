#ifndef VARIANT_H
#define VARIANT_H

#include <string>
#include <variant>

namespace hegadb::types {

using Variant = std::variant<
    bool,
    long long,
    std::string>;

}

#endif // !VARIANT_H
