#ifndef QUERY_PREPARER_H
#define QUERY_PREPARER_H

#include "hegadb/client/ast/ASTNode.h"
#include "hegadb/client/preparation/PreparedQuery.h"
#include "hegadb/crypto/EncryptionWrapper.h"

#include <memory>

namespace hegadb::client::preparation {

class QueryPreparer {
public:
    explicit QueryPreparer(crypto::EncryptionWrapper& encryption);

    PreparedQuery
    prepare(std::unique_ptr<ast::ASTNode> root) const;

private:
    crypto::EncryptionWrapper& encryption_;
};

}

#endif // !QUERY_PREPARER_H
