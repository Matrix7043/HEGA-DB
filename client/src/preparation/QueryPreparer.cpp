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

    // TODO: REPLACE THE BFV AND CKKS PUBLIC KEY WITH ACTUAL SERIALIZE METHODS

    std::string bfv_public_key = "BFV_PUBLIC_KEY";
    std::string ckks_public_key = "CKKS_PUBLIC_KEY";

    return PreparedQuery(
        std::move(parameterizedQuery),
        std::move(parameters),
        std::move(bfv_public_key),
        std::move(ckks_public_key));
}

} // namespace hegadb
