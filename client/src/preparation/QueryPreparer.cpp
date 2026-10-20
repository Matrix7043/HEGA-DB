#include "hegadb/client/preparation/QueryPreparer.h"
#include "hegadb/client/ast/ASTParameterizer.h"
#include "hegadb/client/ast/InsertStmt.h"
#include "hegadb/client/preparation/LiteralExtractor.h"
#include "hegadb/crypto/KeyManager.h"
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

    // INSERT values are encrypted with encryptInsert, which puts one value in
    // slot zero of its ciphertext so the server can place it into a column.
    // Literals of a SELECT are only compared against, so they use encrypt.
    bool const is_insert = dynamic_cast<ast::InsertStmt const*>(root.get()) != nullptr;

    for (auto const& literal : literals) {
        crypto::Plaintext plaintext(literal.value);
        auto ciphertexts = is_insert
            ? encryption_.encryptInsert(plaintext)
            : encryption_.encrypt(plaintext);
        parameters.emplace_back(literal.parameter_index, std::move(ciphertexts));
    }

    if (literals.size() > 0) {
        crypto::KeyManager const& key_manager = encryption_.getKeyManager();
        std::string bfv_public_key = key_manager.serializeBFVPublicKey();
        std::string ckks_public_key = key_manager.serializeCKKSPublicKey();

        return PreparedQuery(
            std::move(parameterizedQuery),
            std::move(parameters),
            std::move(bfv_public_key),
            std::move(ckks_public_key));
    }

    return PreparedQuery(
        std::move(parameterizedQuery),
        std::move(parameters));
}

} // namespace hegadb
