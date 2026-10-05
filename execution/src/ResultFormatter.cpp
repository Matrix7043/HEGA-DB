#include <hegadb/execution/ResultFormatter.h>

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace hegadb::execution {

// ANSI color codes
namespace colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string CYAN = "\033[36m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string GRAY = "\033[90m";
}

ResultFormatter::ResultFormatter(FormatConfig config)
    : config_(std::move(config))
{
}

void ResultFormatter::format(
    QueryResult const& result,
    std::ostream& out)
{
    if (!result.success) {
        out << "Error: " << result.error_message << "\n";
        return;
    }

    if (result.rows.empty()) {
        out << "(0 rows)\n";
        return;
    }

    switch (config_.format) {
        case DisplayFormat::Table:
            formatTable(result.column_names, result.rows, out);
            break;
        case DisplayFormat::CSV:
            formatCSV(result.column_names, result.rows, out);
            break;
        case DisplayFormat::JSON:
            formatJSON(result.column_names, result.rows, out);
            break;
        case DisplayFormat::Compact:
            formatCompact(result.column_names, result.rows, out);
            break;
    }

    if (config_.show_row_count) {
        out << "\n(" << result.rows.size() << " row";
        if (result.rows.size() != 1) out << "s";
        out << ")\n";
    }
}

void ResultFormatter::format(
    DecryptedResult const& result,
    std::ostream& out)
{
    // Convert DecryptedResult to displayable format
    std::vector<std::vector<std::string>> display_rows;
    
    for (auto const& row : result.rows) {
        if (row.matches_filter) {
            display_rows.push_back(row.values);
        }
    }

    if (display_rows.empty()) {
        out << "(0 rows)\n";
        return;
    }

    switch (config_.format) {
        case DisplayFormat::Table:
            formatTable(result.column_names, display_rows, out);
            break;
        case DisplayFormat::CSV:
            formatCSV(result.column_names, display_rows, out);
            break;
        case DisplayFormat::JSON:
            formatJSON(result.column_names, display_rows, out);
            break;
        case DisplayFormat::Compact:
            formatCompact(result.column_names, display_rows, out);
            break;
    }

    if (config_.show_row_count) {
        out << "\n(" << display_rows.size() << " row";
        if (display_rows.size() != 1) out << "s";
        out << ")\n";
    }
}

void ResultFormatter::formatTable(
    std::vector<std::string> const& column_names,
    std::vector<std::vector<std::string>> const& rows,
    std::ostream& out)
{
    auto widths = calculateColumnWidths(column_names, rows);

    // Top border
    out << "+";
    for (auto width : widths) {
        out << std::string(width + 2, '-') << "+";
    }
    out << "\n";

    // Header
    out << "|";
    for (size_t i = 0; i < column_names.size(); ++i) {
        std::string header = truncate(column_names[i], widths[i]);
        if (config_.color_enabled) {
            header = colorize(header, colors::CYAN + colors::BOLD);
        }
        out << " " << std::left << std::setw(widths[i] + (config_.color_enabled ? 9 : 0)) 
            << header << " |";
    }
    out << "\n";

    // Separator
    out << "+";
    for (auto width : widths) {
        out << std::string(width + 2, '=') << "+";
    }
    out << "\n";

    // Rows
    for (auto const& row : rows) {
        out << "|";
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            std::string cell = truncate(row[i], widths[i]);
            out << " " << std::left << std::setw(widths[i]) << cell << " |";
        }
        out << "\n";
    }

    // Bottom border
    out << "+";
    for (auto width : widths) {
        out << std::string(width + 2, '-') << "+";
    }
    out << "\n";
}

void ResultFormatter::formatCSV(
    std::vector<std::string> const& column_names,
    std::vector<std::vector<std::string>> const& rows,
    std::ostream& out)
{
    // Header
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (i > 0) out << ",";
        
        // Quote if contains comma or quote
        std::string value = column_names[i];
        if (value.find(',') != std::string::npos || value.find('"') != std::string::npos) {
            // Escape quotes
            std::string escaped;
            for (char c : value) {
                if (c == '"') escaped += "\"\"";
                else escaped += c;
            }
            out << "\"" << escaped << "\"";
        } else {
            out << value;
        }
    }
    out << "\n";

    // Rows
    for (auto const& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) out << ",";
            
            std::string value = row[i];
            if (value.find(',') != std::string::npos || value.find('"') != std::string::npos) {
                std::string escaped;
                for (char c : value) {
                    if (c == '"') escaped += "\"\"";
                    else escaped += c;
                }
                out << "\"" << escaped << "\"";
            } else {
                out << value;
            }
        }
        out << "\n";
    }
}

void ResultFormatter::formatJSON(
    std::vector<std::string> const& column_names,
    std::vector<std::vector<std::string>> const& rows,
    std::ostream& out)
{
    out << "[\n";
    
    for (size_t row_idx = 0; row_idx < rows.size(); ++row_idx) {
        out << "  {";
        
        for (size_t col_idx = 0; col_idx < column_names.size() && col_idx < rows[row_idx].size(); ++col_idx) {
            if (col_idx > 0) out << ", ";
            
            // JSON key
            out << "\"" << column_names[col_idx] << "\": ";
            
            // JSON value (always quoted for simplicity)
            out << "\"";
            for (char c : rows[row_idx][col_idx]) {
                if (c == '"') out << "\\\"";
                else if (c == '\\') out << "\\\\";
                else if (c == '\n') out << "\\n";
                else if (c == '\t') out << "\\t";
                else out << c;
            }
            out << "\"";
        }
        
        out << "}";
        if (row_idx < rows.size() - 1) out << ",";
        out << "\n";
    }
    
    out << "]\n";
}

void ResultFormatter::formatCompact(
    std::vector<std::string> const& column_names,
    std::vector<std::vector<std::string>> const& rows,
    std::ostream& out)
{
    // Header
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (i > 0) out << " ";
        out << column_names[i];
    }
    out << "\n";

    // Rows
    for (auto const& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) out << " ";
            out << row[i];
        }
        out << "\n";
    }
}

std::vector<size_t> ResultFormatter::calculateColumnWidths(
    std::vector<std::string> const& column_names,
    std::vector<std::vector<std::string>> const& rows)
{
    std::vector<size_t> widths(column_names.size());

    // Start with header widths
    for (size_t i = 0; i < column_names.size(); ++i) {
        widths[i] = column_names[i].length();
    }

    // Check row widths
    for (auto const& row : rows) {
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            widths[i] = std::max(widths[i], row[i].length());
        }
    }

    // Apply max width limit
    for (auto& width : widths) {
        width = std::min(width, config_.max_column_width);
    }

    return widths;
}

std::string ResultFormatter::truncate(std::string const& str, size_t max_width) {
    if (str.length() <= max_width) {
        return str;
    }
    return str.substr(0, max_width - 3) + "...";
}

std::string ResultFormatter::colorize(std::string const& text, std::string const& color_code) {
    if (!config_.color_enabled) {
        return text;
    }
    return color_code + text + colors::RESET;
}

void ResultFormatter::setFormat(DisplayFormat format) {
    config_.format = format;
}

FormatConfig const& ResultFormatter::getConfig() const {
    return config_;
}

} // namespace hegadb::execution
