#include "hegadb/client/preparation/PreparedQuery.h"
#include "hegadb/client/ast/ASTNode.h"
#include <utility>

namespace hegadb::client::preparation {

PreparedQuery::PreparedQuery(
    std::unique_ptr<ast::ASTNode> query,
    std::vector<EncryptedParameter> parameters,
    std::string bfv_public_key,
    std::string ckks_public_key)
    : query_(std::move(query))
    , parameters_(std::move(parameters))
    , bfv_public_key_(std::move(bfv_public_key))
    , ckks_public_key_(std::move(ckks_public_key))
{
}
ast::ASTNode const& PreparedQuery::query() const
{
    return *query_;
}

std::vector<EncryptedParameter> const& PreparedQuery::parameters() const
{
    return parameters_;
}

std::string const& PreparedQuery::bfvPublicKey() const
{
    return bfv_public_key_;
}

std::string const& PreparedQuery::ckksPublicKey() const
{
    return ckks_public_key_;
}

}
