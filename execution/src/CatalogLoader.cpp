#include <hegadb/execution/CatalogLoader.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/server/storage/table_manifest.h>
#include <hegadb/types/DataType.h>
#include <hegadb/logging/Logger.h>

#include <filesystem>
#include <stdexcept>
#include <unordered_map>

namespace hegadb::execution {

namespace {

// Convert the type string written by FileStorageWriter back to DataType.
types::DataType parseDataType(std::string const& type_str)
{
    if (type_str == "INTEGER") return types::DataType::Integer;
    if (type_str == "STRING")  return types::DataType::String;
    if (type_str == "FLOAT")   return types::DataType::Float;
    if (type_str == "BOOLEAN") return types::DataType::Boolean;
    throw std::runtime_error("CatalogLoader: unknown column type '" + type_str + "'");
}

} // namespace

std::shared_ptr<catalog::InMemoryCatalog>
CatalogLoader::loadFromDirectory(std::string const& catalog_dir)
{
    namespace fs = std::filesystem;

    if (!fs::exists(catalog_dir) || !fs::is_directory(catalog_dir)) {
        throw std::runtime_error(
            "CatalogLoader: catalog directory not found: " + catalog_dir);
    }

    std::unordered_map<std::string, catalog::TableSchema> tables;

    for (auto const& entry : fs::directory_iterator(catalog_dir)) {
        if (!entry.is_regular_file()) continue;

        auto const& path = entry.path();
        if (path.extension() != ".json") continue;
        // Expect <TABLE>.manifest.json
        if (path.filename().string().find(".manifest.json") == std::string::npos) continue;

        logging::info("CatalogLoader: reading manifest: {}", path.string());

        auto result = server::storage::TableManifest::deserialize(path.string());
        if (!result.has_value()) {
            logging::warn("CatalogLoader: failed to parse {}, skipping", path.string());
            continue;
        }

        auto const& manifest = *result;

        // Reconstruct the TableSchema from column metadata
        std::vector<catalog::ColumnSchema> cols;
        cols.reserve(manifest.columns.size());
        for (auto const& col_meta : manifest.columns) {
            catalog::ColumnSchema col;
            col.name     = col_meta.name;
            col.type     = parseDataType(col_meta.type);
            col.nullable = false;   // manifests don't store nullability; default false
            col.ordinal  = static_cast<std::size_t>(col_meta.slot_index);
            cols.push_back(std::move(col));
        }

        catalog::TableSchema schema(manifest.table_name, std::move(cols));
        logging::info("CatalogLoader: loaded table '{}' ({} columns, {} rows)",
            manifest.table_name, manifest.columns.size(), manifest.total_records);

        tables.emplace(manifest.table_name, std::move(schema));
    }

    if (tables.empty()) {
        logging::warn("CatalogLoader: no tables found in '{}'", catalog_dir);
    }

    auto snapshot = std::make_shared<catalog::CatalogSnapshot const>(
        1, std::move(tables));
    return std::make_shared<catalog::InMemoryCatalog>(snapshot);
}

} // namespace hegadb::execution
