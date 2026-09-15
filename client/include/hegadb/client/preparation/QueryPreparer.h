#ifndef QUERY_PREPARER_H
#define QUERY_PREPARER_H

#include "ast/ASTNode.h"
#include "crypto/EncryptionWrapper.h"
#include "preparation/PreparedQuery.h"

#include <memory>

namespace hegadb {

class QueryPreparer {
public:
    explicit QueryPreparer(EncryptionWrapper& encryption);

    PreparedQuery
    prepare(std::unique_ptr<ASTNode> root) const;

private:
    EncryptionWrapper& encryption_;
};

}

#endif // !QUERY_PREPARER_H
