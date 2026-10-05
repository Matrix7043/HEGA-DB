#include <hegadb/execution/ExecutionREPL.h>
#include <hegadb/logging/Logger.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <functional>

namespace hegadb::execution {

ExecutionREPL::ExecutionREPL(
    std::shared_ptr<LocalExecutor> executor,
    REPLConfig config)
    : executor_(std::move(executor))
    , config_(std::move(config))
{
    FormatConfig fmt_config;
    fmt_config.format = config_.output_format;
    formatter_ = std::make_unique<ResultFormatter>(fmt_config);
    
    loader_ = std::make_unique<DataLoader>(
        std::shared_ptr<server::storage::StorageManager>(&executor_->getStorageManager(), [](auto*){}),
        std::shared_ptr<crypto::CompositeEncryption>(&executor_->getEncryption(), [](auto*){})
    );
}

int ExecutionREPL::run() {
    displayBanner();

    // Load history
    read_history(config_.history_file.c_str());

    while (true) {
        char* raw_input = readline(config_.prompt.c_str());

        if (raw_input == nullptr) {
            // EOF (Ctrl+D)
            std::cout << "\n";
            break;
        }

        std::string input(raw_input);
        free(raw_input);

        // Skip empty input
        if (input.empty()) {
            continue;
        }

        // Add to history
        add_history(input.c_str());

        // Execute command
        bool should_continue = executeCommand(input);
        
        if (!should_continue) {
            break;
        }
    }

    // Save history
    write_history(config_.history_file.c_str());

    std::cout << "Goodbye!\n";
    return 0;
}

bool ExecutionREPL::executeCommand(std::string const& input) {
    // Check for quit command
    if (input == "\\q" || input == "\\quit" || input == "exit") {
        return false;
    }

    // Check for meta-commands
    if (handleMetaCommand(input)) {
        return true;
    }

    // Must be SQL query
    executeSQLQuery(input);
    return true;
}

void ExecutionREPL::displayBanner() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║         HEGA-DB Local Execution Engine          ║\n";
    std::cout << "║    Homomorphic Encryption Query Testing REPL    ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Type SQL queries or meta-commands:\n";
    std::cout << "  \\help       - Show help\n";
    std::cout << "  \\tables     - List all tables\n";
    std::cout << "  \\schema T   - Show schema for table T\n";
    std::cout << "  \\load ...   - Load CSV data\n";
    std::cout << "  \\format F   - Set output format (table|csv|json|compact)\n";
    std::cout << "  \\timing     - Toggle timing display\n";
    std::cout << "  \\q, \\quit   - Exit\n";
    std::cout << "\n";
}

void ExecutionREPL::displayHelp() {
    std::cout << "\nAvailable Commands:\n";
    std::cout << "==================\n\n";
    
    std::cout << "SQL Queries:\n";
    std::cout << "  SELECT columns FROM table WHERE condition\n\n";
    
    std::cout << "Meta-Commands:\n";
    std::cout << "  \\help                    Show this help message\n";
    std::cout << "  \\tables                  List all tables in catalog\n";
    std::cout << "  \\schema <table>          Show schema for a table\n";
    std::cout << "  \\load <file> <table>     Load CSV file into table\n";
    std::cout << "  \\format <format>         Set output format (table, csv, json, compact)\n";
    std::cout << "  \\timing                  Toggle query timing display\n";
    std::cout << "  \\q, \\quit, exit          Exit the REPL\n\n";
}

bool ExecutionREPL::handleMetaCommand(std::string const& input) {
    if (input[0] != '\\') {
        return false;
    }

    std::istringstream iss(input);
    std::string command;
    iss >> command;

    if (command == "\\help" || command == "\\h") {
        displayHelp();
        return true;
    }

    if (command == "\\tables" || command == "\\t") {
        handleTablesCommand();
        return true;
    }

    if (command == "\\schema" || command == "\\d") {
        std::string args;
        std::getline(iss, args);
        handleSchemaCommand(args);
        return true;
    }

    if (command == "\\load") {
        std::string args;
        std::getline(iss, args);
        handleLoadCommand(args);
        return true;
    }

    if (command == "\\format") {
        std::string args;
        std::getline(iss, args);
        handleFormatCommand(args);
        return true;
    }

    if (command == "\\timing") {
        handleTimingCommand();
        return true;
    }

    std::cout << "Unknown command: " << command << "\n";
    std::cout << "Type \\help for help.\n";
    return true;
}

void ExecutionREPL::handleLoadCommand(std::string const& args) {
    std::istringstream iss(args);
    std::string csv_path, table_name;
    
    iss >> csv_path >> table_name;

    if (csv_path.empty() || table_name.empty()) {
        std::cout << "Usage: \\load <csv_file> <table_name>\n";
        return;
    }

    std::cout << "Loading data from " << csv_path << " into table " << table_name << "...\n";

    // Find table schema
    auto* schema = executor_->getCatalog().findTable(table_name);
    if (!schema) {
        std::cout << "Error: Table '" << table_name << "' not found in catalog.\n";
        std::cout << "Create the schema first or use a different table name.\n";
        return;
    }

    // Load CSV
    auto result = loader_->loadCSV(csv_path, *schema);

    if (result.success) {
        std::cout << "Success! Loaded " << result.rows_loaded << " rows.\n";
    } else {
        std::cout << "Error: " << result.error_message << "\n";
        if (result.rows_failed > 0) {
            std::cout << "Failed rows: " << result.rows_failed << "\n";
        }
    }
}

void ExecutionREPL::handleSchemaCommand(std::string const& args) {
    std::istringstream iss(args);
    std::string table_name;
    iss >> table_name;

    if (table_name.empty()) {
        std::cout << "Usage: \\schema <table_name>\n";
        return;
    }

    auto* schema = executor_->getCatalog().findTable(table_name);
    if (!schema) {
        std::cout << "Table not found: " << table_name << "\n";
        return;
    }

    std::cout << "\nTable: " << schema->name() << "\n";
    std::cout << "Columns:\n";
    
    for (auto const& col : schema->columns()) {
        std::cout << "  " << std::left << std::setw(20) << col.name;
        
        switch (col.type) {
            case types::DataType::Integer:
                std::cout << "INTEGER";
                break;
            case types::DataType::Float:
                std::cout << "FLOAT";
                break;
            case types::DataType::String:
                std::cout << "STRING";
                break;
            case types::DataType::Boolean:
                std::cout << "BOOLEAN";
                break;
            default:
                std::cout << "UNKNOWN";
        }
        
        std::cout << "\n";
    }
    std::cout << "\n";
}

void ExecutionREPL::handleTablesCommand() {
    std::cout << "\nTables:\n";
    
    // Note: Catalog interface doesn't provide a list method
    // This is a placeholder - you may need to extend the Catalog interface
    std::cout << "  (Catalog list not yet implemented)\n";
    std::cout << "  Use \\schema <table_name> to inspect a specific table\n\n";
}

void ExecutionREPL::handleFormatCommand(std::string const& args) {
    std::istringstream iss(args);
    std::string format_str;
    iss >> format_str;

    if (format_str.empty()) {
        std::cout << "Current format: ";
        switch (formatter_->getConfig().format) {
            case DisplayFormat::Table: std::cout << "table\n"; break;
            case DisplayFormat::CSV: std::cout << "csv\n"; break;
            case DisplayFormat::JSON: std::cout << "json\n"; break;
            case DisplayFormat::Compact: std::cout << "compact\n"; break;
        }
        return;
    }

    if (format_str == "table") {
        formatter_->setFormat(DisplayFormat::Table);
        std::cout << "Output format set to: table\n";
    } else if (format_str == "csv") {
        formatter_->setFormat(DisplayFormat::CSV);
        std::cout << "Output format set to: csv\n";
    } else if (format_str == "json") {
        formatter_->setFormat(DisplayFormat::JSON);
        std::cout << "Output format set to: json\n";
    } else if (format_str == "compact") {
        formatter_->setFormat(DisplayFormat::Compact);
        std::cout << "Output format set to: compact\n";
    } else {
        std::cout << "Unknown format: " << format_str << "\n";
        std::cout << "Available formats: table, csv, json, compact\n";
    }
}

void ExecutionREPL::handleTimingCommand() {
    config_.show_timing = !config_.show_timing;
    std::cout << "Timing display: " << (config_.show_timing ? "ON" : "OFF") << "\n";
}

void ExecutionREPL::executeSQLQuery(std::string const& sql) {
    if (config_.echo_queries) {
        std::cout << "Executing: " << sql << "\n\n";
    }

    double execution_time = 0.0;
    QueryResult result;

    if (config_.show_timing) {
        auto start = std::chrono::high_resolution_clock::now();
        result = executor_->execute(sql);
        auto end = std::chrono::high_resolution_clock::now();
        
        execution_time = std::chrono::duration<double, std::milli>(end - start).count();
    } else {
        result = executor_->execute(sql);
    }

    // Display result
    formatter_->format(result, std::cout);

    // Display timing if enabled
    if (config_.show_timing) {
        std::cout << "\nExecution time: " << std::fixed << std::setprecision(2) 
                  << execution_time << " ms\n";
    }

    std::cout << "\n";
}

} // namespace hegadb::execution
