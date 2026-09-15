#ifndef PREPARED_QUERY_H
#define PREPARED_QUERY_H

#include "hegadb/client/ast/ASTNode.h"
#include "hegadb/client/preparation/EncryptedParameter.h"

#include <memory>
#include <vector>

namespace hegadb::client::preparation {

class PreparedQuery {
public:
    PreparedQuery(
        std::unique_ptr<ast::ASTNode> query,
        std::vector<EncryptedParameter> parameters);

    ast::ASTNode const& query() const;
    std::vector<EncryptedParameter> const& parameters() const;

private:
    std::unique_ptr<ast::ASTNode> query_;
    std::vector<EncryptedParameter> parameters_;
};

}

#endif // !PREPARED_QUERY_H
