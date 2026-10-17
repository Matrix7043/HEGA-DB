#pragma once

#include <hegadb/catalog/db-client/InMemoryCatalog.h>

#include <memory>
#include <string>

namespace hegadb::execution {

/**
 * @brief Builds an InMemoryCatalog by scanning a catalog directory.
 *
 * For every *.manifest.json file found in @p catalog_dir, deserializes the
 * TableManifest and reconstructs the matching TableSchema so that the catalog
 * reflects the actual tables that exist on disk — without any hardcoded schema.
 */
class CatalogLoader {
public:
    /**
     * @brief Load all tables from @p catalog_dir into a new InMemoryCatalog.
     * @throws std::runtime_error if the directory cannot be read or any
     *         manifest is malformed.
     */
    static std::shared_ptr<catalog::InMemoryCatalog>
    loadFromDirectory(std::string const& catalog_dir);
};

} // namespace hegadb::execution
