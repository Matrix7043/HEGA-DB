#ifndef HEGADB_EXECUTION_REPL_H
#define HEGADB_EXECUTION_REPL_H

#include <hegadb/execution/LocalExecutor.h>
#include <hegadb/execution/ResultFormatter.h>
#include <hegadb/execution/DataLoader.h>

#include <memory>
#include <string>
#include <functional>

namespace hegadb::execution {

/**
 * @brief REPL configuration
 */
struct REPLConfig {
    std::string prompt = "hegadb> ";
    std::string history_file = ".hegadb_history";
    bool echo_queries = false;
    bool show_timing = true;
    DisplayFormat output_format = DisplayFormat::Table;
};

/**
 * @brief Interactive SQL shell for local query execution
 * 
 * ExecutionREPL provides:
 * - Read-Eval-Print Loop for SQL queries
 * - Meta-commands for control (\load, \schema, \quit, etc.)
 * - Query history with readline
 * - Timing and statistics
 * - Error handling and user-friendly messages
 */
class ExecutionREPL {
public:
    /**
     * @brief Constructor
     * @param executor Local executor for query execution
     * @param config REPL configuration
     */
    ExecutionREPL(
        std::shared_ptr<LocalExecutor> executor,
        REPLConfig config = REPLConfig{}
    );

    /**
     * @brief Start the REPL loop
     * @return Exit code (0 = success)
     */
    int run();

    /**
     * @brief Execute a single command/query
     * @param input Command or SQL query
     * @return true if should continue, false if should exit
     */
    bool executeCommand(std::string const& input);

private:
    /**
     * @brief Display welcome banner
     */
    void displayBanner();

    /**
     * @brief Display help message
     */
    void displayHelp();

    /**
     * @brief Handle meta-commands (\load, \schema, etc.)
     * @return true if command was handled, false if not a meta-command
     */
    bool handleMetaCommand(std::string const& input);

    /**
     * @brief Handle \load command to load CSV data
     */
    void handleLoadCommand(std::string const& args);

    /**
     * @brief Handle \schema command to display table schema
     */
    void handleSchemaCommand(std::string const& args);

    /**
     * @brief Handle \tables command to list all tables
     */
    void handleTablesCommand();

    /**
     * @brief Handle \format command to change output format
     */
    void handleFormatCommand(std::string const& args);

    /**
     * @brief Handle \timing command to toggle timing display
     */
    void handleTimingCommand();

    /**
     * @brief Execute SQL query
     */
    void executeSQLQuery(std::string const& sql);

    std::shared_ptr<LocalExecutor> executor_;
    std::unique_ptr<ResultFormatter> formatter_;
    std::unique_ptr<DataLoader> loader_;
    REPLConfig config_;
};

} // namespace hegadb::execution

#endif // HEGADB_EXECUTION_REPL_H
