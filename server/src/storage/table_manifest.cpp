#include "hegadb/server/storage/table_manifest.h"
#include <hegadb/logging/Logger.h>
#include <cerrno>
#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <cctype>

// Totally legit and not one bit AI generated json parser & serializer
// Please use this in prod
// TODO: Replace this with a json library in the future

namespace hegadb::server::storage {

StorageResult TableManifest::serialize(std::string const& file_path) const
{
    std::string tmp_file = file_path + ".tmp";
    hegadb::logging::debug("Serializing manifest for table '{}' to temporary file '{}'", table_name, tmp_file);
    std::ofstream out(tmp_file);
    if (!out) {
        hegadb::logging::error("Failed to open temporary manifest file for writing: '{}' (errno={})", tmp_file, errno);
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
        hegadb::logging::error("Failed atomic rename for manifest: '{}' -> '{}' (errno={})", tmp_file, file_path, errno);
        return StorageError::IOError;
    }

    hegadb::logging::info("Successfully serialized manifest for table '{}' to '{}' (columns={}, segments={}, total_records={})",
                          table_name, file_path, columns.size(), segments.size(), total_records);

    return {};
}

// Simple parsing based on known structure (very fragile, just for mock purposes)
ValueResult<TableManifest> TableManifest::deserialize(std::string const& file_path)
{
    hegadb::logging::debug("Deserializing table manifest from file: '{}'", file_path);
    std::ifstream in(file_path);
    if (!in) {
        hegadb::logging::error("Failed to open table manifest file for reading: '{}'", file_path);
        return StorageError::FileNotFound;
    }

    TableManifest manifest;
    std::string line;

    // Simple line-by-line parser for our JSON format
    while (std::getline(in, line)) {
        // Remove leading/trailing whitespace
        size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos) continue;
        line = line.substr(first);
        
        // Parse table_name
        if (line.find("\"table_name\":") != std::string::npos) {
            size_t start = line.find(":") + 1;
            size_t quote1 = line.find("\"", start);
            size_t quote2 = line.find("\"", quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos) {
                manifest.table_name = line.substr(quote1 + 1, quote2 - quote1 - 1);
            }
        }
        
        // Parse total_records
        if (line.find("\"total_records\":") != std::string::npos) {
            size_t start = line.find(":") + 1;
            size_t comma = line.find(",", start);
            std::string value_str = line.substr(start, comma - start);
            // Remove whitespace
            value_str.erase(remove_if(value_str.begin(), value_str.end(), ::isspace), value_str.end());
            try {
                manifest.total_records = std::stoull(value_str);
            } catch (...) {
                manifest.total_records = 0;
            }
        }
        
        // Parse columns (simplified - just extract name and type)
        if (line.find("\"name\":") != std::string::npos && line.find("\"type\":") != std::string::npos) {
            ColumnMetadata col;
            
            // Extract name
            size_t name_start = line.find("\"name\":");
            if (name_start != std::string::npos) {
                size_t quote1 = line.find("\"", name_start + 7);
                size_t quote2 = line.find("\"", quote1 + 1);
                if (quote1 != std::string::npos && quote2 != std::string::npos) {
                    col.name = line.substr(quote1 + 1, quote2 - quote1 - 1);
                }
            }
            
            // Extract type
            size_t type_start = line.find("\"type\":");
            if (type_start != std::string::npos) {
                size_t quote1 = line.find("\"", type_start + 7);
                size_t quote2 = line.find("\"", quote1 + 1);
                if (quote1 != std::string::npos && quote2 != std::string::npos) {
                    col.type = line.substr(quote1 + 1, quote2 - quote1 - 1);
                }
            }
            
            // Extract scheme
            size_t scheme_start = line.find("\"scheme\":");
            if (scheme_start != std::string::npos) {
                size_t quote1 = line.find("\"", scheme_start + 9);
                size_t quote2 = line.find("\"", quote1 + 1);
                if (quote1 != std::string::npos && quote2 != std::string::npos) {
                    col.scheme = line.substr(quote1 + 1, quote2 - quote1 - 1);
                }
            }
            
            // Extract slot_index
            size_t slot_start = line.find("\"slot_index\":");
            if (slot_start != std::string::npos) {
                size_t colon = line.find(":", slot_start);
                size_t end = line.find_first_of(",}", colon);
                std::string value_str = line.substr(colon + 1, end - colon - 1);
                value_str.erase(remove_if(value_str.begin(), value_str.end(), ::isspace), value_str.end());
                try {
                    col.slot_index = std::stoul(value_str);
                } catch (...) {
                    col.slot_index = 0;
                }
            }
            
            if (!col.name.empty() && !col.type.empty()) {
                manifest.columns.push_back(col);
            }
        }
        
        // Parse segments — each segment block looks like:
        //   "col_name": {
        //     "file": "col_name.seg",
        //     ...
        //   }
        // We need to identify the key (column name) from the outer object key.
        // Strategy: when we see a line that is only a quoted key + colon + "{",
        // that's a segment key. We then read the block and associate the key.
        //
        // Detect: line matches pattern   "some_name": {
        // (only inside the segments object, i.e. after we've parsed all columns)
        bool looks_like_segment_key =
            !manifest.columns.empty() &&
            line.size() >= 5 &&
            line.front() == '"' &&
            line.find("\"file\":") == std::string::npos &&   // not the file line
            line.find("\"name\":") == std::string::npos &&   // not a column line
            line.find("\"type\":") == std::string::npos &&
            line.find("\"segments\"") == std::string::npos &&
            line.find("\"columns\"") == std::string::npos &&
            line.find("\"dictionary\"") == std::string::npos &&
            line.find("{") != std::string::npos;

        if (looks_like_segment_key) {
            // Extract the key name (the column name for this segment)
            size_t q1 = line.find("\"");
            size_t q2 = line.find("\"", q1 + 1);
            std::string seg_col_name;
            if (q1 != std::string::npos && q2 != std::string::npos) {
                seg_col_name = line.substr(q1 + 1, q2 - q1 - 1);
            }

            if (seg_col_name.empty()) continue;

            SegmentMetadata seg;

            // Accumulate the block lines until closing "}"
            std::string seg_block;
            while (std::getline(in, line)) {
                // Check for closing brace of this segment block
                if (line.find("}") != std::string::npos) break;
                seg_block += line + "\n";
            }

            auto extract_str = [&](std::string const& key) -> std::string {
                size_t pos = seg_block.find(key);
                if (pos == std::string::npos) return "";
                size_t colon = seg_block.find(":", pos);
                if (colon == std::string::npos) return "";
                size_t q1 = seg_block.find("\"", colon);
                size_t q2 = seg_block.find("\"", q1 + 1);
                if (q1 == std::string::npos || q2 == std::string::npos) return "";
                return seg_block.substr(q1 + 1, q2 - q1 - 1);
            };

            auto extract_uint64 = [&](std::string const& key, uint64_t def) -> uint64_t {
                size_t pos = seg_block.find(key);
                if (pos == std::string::npos) return def;
                size_t colon = seg_block.find(":", pos);
                if (colon == std::string::npos) return def;
                size_t end = seg_block.find_first_of(",}\n", colon + 1);
                std::string v = seg_block.substr(colon + 1, end - colon - 1);
                v.erase(remove_if(v.begin(), v.end(), ::isspace), v.end());
                try { return std::stoull(v); } catch (...) { return def; }
            };

            seg.file           = extract_str("\"file\"");
            seg.poly_N         = static_cast<uint32_t>(extract_uint64("\"poly_N\"", 16384));
            seg.param_scale_or_p = extract_uint64("\"param_scale_or_p\"", 65537);
            seg.rows_per_block = static_cast<uint32_t>(extract_uint64("\"rows_per_block\"", 1));
            seg.total_blocks   = extract_uint64("\"total_blocks\"", 1);

            if (!seg.file.empty()) {
                manifest.segments[seg_col_name] = seg;
            }
        }
    }

    hegadb::logging::info("Successfully loaded manifest for table '{}' from '{}'", manifest.table_name, file_path);
    return manifest;
}

} // namespace hegadb::server::storage
