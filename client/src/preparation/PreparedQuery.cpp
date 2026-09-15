#include "ast/ASTNode.h"
#include <preparation/PreparedQuery.h>
#include <utility>

namespace hegadb {

PreparedQuery::PreparedQuery(
    std::unique_ptr<ASTNode> query,
    std::vector<EncryptedParameter> parameters)
    : query_(std::move(query))
    , parameters_(std::move(parameters))
{
}
ASTNode const& PreparedQuery::query() const { return *query_; }

std::vector<EncryptedParameter> const& PreparedQuery::parameters() const { return parameters_; }

}
