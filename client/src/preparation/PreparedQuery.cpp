#include "hegadb/client/preparation/PreparedQuery.h"
#include "hegadb/client/ast/ASTNode.h"
#include <utility>

namespace hegadb::client::preparation {

PreparedQuery::PreparedQuery(
    std::unique_ptr<ast::ASTNode> query,
    std::vector<EncryptedParameter> parameters)
    : query_(std::move(query))
    , parameters_(std::move(parameters))
{
}
ast::ASTNode const& PreparedQuery::query() const { return *query_; }

std::vector<EncryptedParameter> const& PreparedQuery::parameters() const { return parameters_; }

}
