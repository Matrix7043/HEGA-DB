#ifndef HEGADB_TOOLS_SETUP_FILE_STORAGE_WRITER_H
#define HEGADB_TOOLS_SETUP_FILE_STORAGE_WRITER_H

#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/OpenFHECiphertext.h>
#include <hegadb/types/DataType.h>
#include <hegadb/server/storage/segment_writer.h>
#include <hegadb/server/storage/table_manifest.h>
#include <hegadb/server/storage/storage_format.h>

#include <string>
#include <vector>

namespace hegadb::tools::setup {

/**
 * @brief Writes encrypted table data using the existing storage format
 * 
 * Uses SegmentWriter and TableManifest from server/storage module
 * to create properly formatted encrypted segments compatible with
 * the server's storage engine.
 */
class FileStorageWriter {
public:
    /**
     * @brief Construct storage writer for a base data directory
     * @param base_dir Base directory (e.g., ~/.hegadb/data/)
     */
    explicit FileStorageWriter(std::string base_dir);

    /**
     * @brief Create a table directory and write encrypted columns as segments
     * @param table_name Name of the table
     * @param column_names Names of columns
     * @param column_types Data types of columns
     * @param encrypted_columns Encrypted ciphertext per column
     * @param row_count Number of rows in the table
     */
    void writeTable(
        std::string const& table_name,
        std::vector<std::string> const& column_names,
        std::vector<types::DataType> const& column_types,
        std::vector<crypto::OpenFHECiphertext> const& encrypted_columns,
        size_t row_count
    );

    /**
     * @brief Write table manifest (metadata) to JSON file
     * @param schema Table schema
     * @param catalog_dir Directory for catalog files
     * @param row_count Number of rows
     */
    static void writeManifest(
        catalog::TableSchema const& schema,
        std::string const& catalog_dir,
        size_t row_count
    );

    /**
     * @brief Get the full path for a table's data directory
     */
    std::string getTablePath(std::string const& table_name) const;

private:
    /**
     * @brief Write a single encrypted column as a segment file
     * @param column_name Name of the column
     * @param column_type Data type
     * @param ciphertext Encrypted data
     * @param row_count Number of rows
     * @param output_path Full path to output segment file
     */
    void writeColumnSegment(
        std::string const& column_name,
        types::DataType column_type,
        crypto::OpenFHECiphertext const& ciphertext,
        size_t row_count,
        std::string const& output_path
    );

    /**
     * @brief Ensure directory exists, create if needed
     */
    static void ensureDirectoryExists(std::string const& path);

    std::string base_dir_;
};

} // namespace hegadb::tools::setup

#endif // HEGADB_TOOLS_SETUP_FILE_STORAGE_WRITER_H
