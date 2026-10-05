#ifndef HEGADB_EXECUTION_RESULT_FORMATTER_H
#define HEGADB_EXECUTION_RESULT_FORMATTER_H

#include <hegadb/execution/LocalExecutor.h>
#include <hegadb/execution/ResultDecryptor.h>

#include <iostream>
#include <string>
#include <vector>

namespace hegadb::execution {

/**
 * @brief Display format for results
 */
enum class DisplayFormat {
    Table,      // ASCII table with borders
    CSV,        // Comma-separated values
    JSON,       // JSON array of objects
    Compact     // Space-separated, no borders
};

/**
 * @brief Configuration for result display
 */
struct FormatConfig {
    DisplayFormat format = DisplayFormat::Table;
    size_t max_column_width = 50;
    bool show_row_count = true;
    bool show_execution_time = false;
    bool color_enabled = true;
};

/**
 * @brief Pretty-prints query results in various formats
 * 
 * ResultFormatter provides:
 * - ASCII table formatting with borders
 * - CSV output for export
 * - JSON output for programmatic use
 * - Compact format for quick viewing
 * - Color support for terminal output
 */
class ResultFormatter {
public:
    /**
     * @brief Constructor with configuration
     */
    explicit ResultFormatter(FormatConfig config = FormatConfig{});

    /**
     * @brief Format and display query result
     * @param result Query result to display
     * @param out Output stream (default: std::cout)
     */
    void format(
        QueryResult const& result,
        std::ostream& out = std::cout
    );

    /**
     * @brief Format and display decrypted result
     * @param result Decrypted result to display
     * @param out Output stream (default: std::cout)
     */
    void format(
        DecryptedResult const& result,
        std::ostream& out = std::cout
    );

    /**
     * @brief Set display format
     */
    void setFormat(DisplayFormat format);

    /**
     * @brief Get current configuration
     */
    FormatConfig const& getConfig() const;

private:
    /**
     * @brief Format as ASCII table
     */
    void formatTable(
        std::vector<std::string> const& column_names,
        std::vector<std::vector<std::string>> const& rows,
        std::ostream& out
    );

    /**
     * @brief Format as CSV
     */
    void formatCSV(
        std::vector<std::string> const& column_names,
        std::vector<std::vector<std::string>> const& rows,
        std::ostream& out
    );

    /**
     * @brief Format as JSON
     */
    void formatJSON(
        std::vector<std::string> const& column_names,
        std::vector<std::vector<std::string>> const& rows,
        std::ostream& out
    );

    /**
     * @brief Format as compact (space-separated)
     */
    void formatCompact(
        std::vector<std::string> const& column_names,
        std::vector<std::vector<std::string>> const& rows,
        std::ostream& out
    );

    /**
     * @brief Calculate column widths for table formatting
     */
    std::vector<size_t> calculateColumnWidths(
        std::vector<std::string> const& column_names,
        std::vector<std::vector<std::string>> const& rows
    );

    /**
     * @brief Truncate string if exceeds max width
     */
    std::string truncate(std::string const& str, size_t max_width);

    /**
     * @brief Apply color to text (if enabled)
     */
    std::string colorize(std::string const& text, std::string const& color_code);

    FormatConfig config_;
};

} // namespace hegadb::execution

#endif // HEGADB_EXECUTION_RESULT_FORMATTER_H
