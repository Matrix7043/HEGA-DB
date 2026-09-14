#include "preparation/QueryPreparer.h"
#include "crypto/Plaintext.h"
#include "preparation/LiteralExtractor.h"

#include <memory>
#include <utility>
#include <vector>

namespace hegadb {

QueryPreparer::QueryPreparer(
    EncryptionWrapper& encryption)
    : encryption_(encryption)
{
}

PreparedQuery
QueryPreparer::prepare(
    std::unique_ptr<ASTNode> root) const
{
    LiteralExtractor extractor;

    auto literals = extractor.extract(*root);

    std::vector<EncryptedParameter> parameters;
    parameters.reserve(literals.size());

    for (auto const& literal : literals) {
        Plaintext plaintext(literal.value);
        auto ciphertexts = encryption_.encrypt(plaintext);
        parameters.emplace_back(literal.parameter_index, std::move(ciphertexts));
    }

    return PreparedQuery(
        std::move(root),
        std::move(parameters));
}

} // namespace hegadb
