#ifndef PREPARED_QUERY_H
#define PREPARED_QUERY_H

#include "ast/ASTNode.h"
#include "preparation/EncryptedParameter.h"

#include <memory>
#include <vector>

namespace hegadb {

class PreparedQuery {
public:
    PreparedQuery(
        std::unique_ptr<ASTNode> query,
        std::vector<EncryptedParameter> parameters);

    ASTNode const& query() const;
    std::vector<EncryptedParameter> const& parameters() const;

private:
    std::unique_ptr<ASTNode> query_;
    std::vector<EncryptedParameter> parameters_;
};

}

#endif // !PREPARED_QUERY_H
