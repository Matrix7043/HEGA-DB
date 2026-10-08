#ifndef CATALOG_SERVER_TRANSPORT_H
#define CATALOG_SERVER_TRANSPORT_H

namespace hegadb::transport {

class CatalogServerTransport {
public:
    virtual ~CatalogServerTransport() = default;
    virtual void start() = 0;
    virtual void shutdown() = 0;
};

}

#endif // !CATALOG_SERVER_TRANSPORT_H
