#include "hegadb/client/preparation/QueryPreparer.h"
#include "hegadb/client/ast/ASTParameterizer.h"
#include "hegadb/client/preparation/LiteralExtractor.h"
#include "hegadb/crypto/Plaintext.h"

#include <memory>
#include <utility>
#include <vector>

namespace hegadb::client::preparation {

QueryPreparer::QueryPreparer(
    crypto::EncryptionWrapper& encryption)
    : encryption_(encryption)
{
}

PreparedQuery
QueryPreparer::prepare(
    std::unique_ptr<ast::ASTNode> root) const
{
    LiteralExtractor extractor;

    auto literals = extractor.extract(*root);

    ast::ASTParameterizer parameterizer;
    auto parameterizedQuery = parameterizer.parameterize(*root);

    std::vector<EncryptedParameter> parameters;
    parameters.reserve(literals.size());

    for (auto const& literal : literals) {
        crypto::Plaintext plaintext(literal.value);
        auto ciphertexts = encryption_.encrypt(plaintext);
        parameters.emplace_back(literal.parameter_index, std::move(ciphertexts));
    }

    return PreparedQuery(
        std::move(parameterizedQuery),
        std::move(parameters));
}

} // namespace hegadb
