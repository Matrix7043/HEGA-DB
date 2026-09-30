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
        std::vector<EncryptedParameter> parameters,
        std::string bfv_public_key,
        std::string ckks_public_key);

    ast::ASTNode const& query() const;
    std::vector<EncryptedParameter> const& parameters() const;
    std::string const& bfvPublicKey() const;
    std::string const& ckksPublicKey() const;

private:
    std::unique_ptr<ast::ASTNode> query_;
    std::vector<EncryptedParameter> parameters_;
    std::string bfv_public_key_;
    std::string ckks_public_key_;
};

}

#endif // !PREPARED_QUERY_H
