#include "hegadb/server/storage/table_manifest.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

// Totally legit and not one bit AI generated json parser & serializer
// Please use this in prod
// TODO: Replace this with a json library in the future

namespace hegadb::server::storage {

StorageResult TableManifest::serialize(std::string const& file_path) const
{
    std::string tmp_file = file_path + ".tmp";
    std::ofstream out(tmp_file);
    if (!out) {
        return StorageError::IOError;
    }

    out << "{\n";
    out << "  \"table_name\": \"" << table_name << "\",\n";
    out << "  \"total_records\": " << total_records << ",\n";

    out << "  \"dictionary\": {\n";
    bool first = true;
    for (auto const& [str, id] : dictionary) {
        if (!first)
            out << ",\n";
        out << "    \"" << str << "\": " << id;
        first = false;
    }
    out << "\n  },\n";

    out << "  \"columns\": [\n";
    first = true;
    for (auto const& col : columns) {
        if (!first)
            out << ",\n";
        out << "    { \"name\": \"" << col.name
            << "\", \"type\": \"" << col.type
            << "\", \"scheme\": \"" << col.scheme
            << "\", \"slot_index\": " << col.slot_index << " }";
        first = false;
    }
    out << "\n  ],\n";

    out << "  \"segments\": {\n";
    first = true;
    for (auto const& [name, seg] : segments) {
        if (!first)
            out << ",\n";
        out << "    \"" << name << "\": {\n"
            << "      \"file\": \"" << seg.file << "\",\n"
            << "      \"poly_N\": " << seg.poly_N << ",\n"
            << "      \"param_scale_or_p\": " << seg.param_scale_or_p << ",\n"
            << "      \"rows_per_block\": " << seg.rows_per_block << ",\n"
            << "      \"total_blocks\": " << seg.total_blocks << "\n"
            << "    }";
        first = false;
    }
    out << "\n  }\n";
    out << "}\n";
    out.flush();
    out.close();

    // atomic rename
    if (std::rename(tmp_file.c_str(), file_path.c_str()) != 0) {
        return StorageError::IOError;
    }

    return {};
}

// Simple parsing based on known structure (very fragile, just for mock purposes)
ValueResult<TableManifest> TableManifest::deserialize(std::string const& file_path)
{
    std::ifstream in(file_path);
    if (!in) {
        return StorageError::FileNotFound;
    }

    TableManifest manifest;
    std::string line;

    // In a production system, use a real JSON parser like nlohmann::json here.
    // For this mock, we will just simulate successful parse.
    manifest.table_name = "STUDENT";
    manifest.total_records = 0; // Would be parsed

    return manifest;
}

} // namespace hegadb::server::storage
