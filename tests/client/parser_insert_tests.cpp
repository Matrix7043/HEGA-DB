#include <hegadb/client/SQLParser.h>

#include <iostream>
#include <string>
#include <vector>

// Grammar-level tests for INSERT: does the SQL parse or not.
// What the parsed statement means (types, column counts, NOT NULL)
// is checked later, by the semantic validator.

struct TestCase {
    std::string name;
    std::string sql;
    bool shouldPass;
};

int main()
{
    std::vector<TestCase> const tests = {

        // ---------------- Valid INSERT ----------------

        { "INSERT without column list",
            "INSERT INTO users VALUES (1, 'alice');",
            true },

        { "INSERT with column list",
            "INSERT INTO users (id, name) VALUES (1, 'alice');",
            true },

        { "INSERT with a single column and value",
            "INSERT INTO users (id) VALUES (1);",
            true },

        { "INSERT without semicolon",
            "INSERT INTO users VALUES (1, 'alice')",
            true },

        { "INSERT keywords are case-insensitive",
            "insert into users values (1, 'alice');",
            true },

        { "INSERT with mixed-case keywords",
            "InSeRt InTo users VaLuEs (1, 'alice');",
            true },

        { "INSERT with multiple rows",
            "INSERT INTO users VALUES (1, 'alice'), (2, 'bob'), (3, 'carol');",
            true },

        { "INSERT with column list and multiple rows",
            "INSERT INTO users (id, name) VALUES (1, 'alice'), (2, 'bob');",
            true },

        { "INSERT with every literal kind",
            "INSERT INTO t VALUES (42, 'text', TRUE, FALSE);",
            true },

        { "INSERT with NULL",
            "INSERT INTO users VALUES (1, NULL);",
            true },

        { "INSERT with lowercase null and booleans",
            "INSERT INTO t VALUES (null, true, false);",
            true },

        { "INSERT with only NULL values",
            "INSERT INTO t (a, b) VALUES (NULL, NULL);",
            true },

        { "INSERT with an escaped quote in a string",
            "INSERT INTO users VALUES (1, 'it''s');",
            true },

        { "INSERT with an empty string",
            "INSERT INTO users VALUES (1, '');",
            true },

        { "INSERT spread over several lines",
            "INSERT INTO users\n"
            "    (id, name)\n"
            "VALUES\n"
            "    (1, 'alice'),\n"
            "    (2, 'bob');\n",
            true },

        { "INSERT with comments",
            "INSERT INTO users -- the table\n"
            "VALUES /* one row */ (1, 'alice');",
            true },

        { "INSERT with no spaces around punctuation",
            "INSERT INTO users(id,name)VALUES(1,'alice'),(2,'bob');",
            true },

        // ---------------- Invalid INSERT ----------------

        { "Missing INTO",
            "INSERT users VALUES (1, 'alice');",
            false },

        { "Missing VALUES",
            "INSERT INTO users (1, 'alice');",
            false },

        { "Missing VALUES after column list",
            "INSERT INTO users (id, name);",
            false },

        { "Missing table name",
            "INSERT INTO VALUES (1, 'alice');",
            false },

        { "Missing value list",
            "INSERT INTO users VALUES;",
            false },

        { "Value list without parentheses",
            "INSERT INTO users VALUES 1, 'alice';",
            false },

        { "Empty value list",
            "INSERT INTO users VALUES ();",
            false },

        { "Trailing comma in value list",
            "INSERT INTO users VALUES (1, 'alice',);",
            false },

        { "Leading comma in value list",
            "INSERT INTO users VALUES (, 1);",
            false },

        { "Missing comma in value list",
            "INSERT INTO users VALUES (1 'alice');",
            false },

        { "Empty column list",
            "INSERT INTO users () VALUES (1);",
            false },

        { "Trailing comma in column list",
            "INSERT INTO users (id,) VALUES (1);",
            false },

        { "Unclosed column list",
            "INSERT INTO users (id, name VALUES (1, 'alice');",
            false },

        { "Unclosed value list",
            "INSERT INTO users VALUES (1, 'alice';",
            false },

        { "Trailing comma after last row",
            "INSERT INTO users VALUES (1, 'alice'),;",
            false },

        { "Rows not separated by a comma",
            "INSERT INTO users VALUES (1, 'alice') (2, 'bob');",
            false },

        { "Empty row between rows",
            "INSERT INTO users VALUES (1, 'alice'), , (2, 'bob');",
            false },

        { "Tokens after the statement",
            "INSERT INTO users VALUES (1, 'alice') extra;",
            false },

        { "Column name as a value",
            "INSERT INTO users VALUES (id, 'alice');",
            false },

        { "Qualified column in the column list",
            "INSERT INTO users (users.id) VALUES (1);",
            false },

        { "Expression as a value",
            "INSERT INTO users VALUES (1 + 2);",
            false },

        { "NOT NULL as a value",
            "INSERT INTO users VALUES (NOT NULL);",
            false },

        { "Parenthesised value",
            "INSERT INTO users VALUES ((1));",
            false },

        { "Unterminated string",
            "INSERT INTO users VALUES (1, 'alice);",
            false },

        // Not supported by the language yet. These guard the current
        // behaviour so a future change to the literals is deliberate.
        { "Float literal is not supported",
            "INSERT INTO t VALUES (1.5);",
            false },

        { "Negative number is not supported",
            "INSERT INTO t VALUES (-1);",
            false },

        // The new keywords are reserved.
        { "INSERT is reserved as a table name",
            "SELECT * FROM insert;",
            false },

        { "VALUES is reserved as a table name",
            "INSERT INTO values VALUES (1);",
            false },

        { "INTO is reserved as a column name",
            "INSERT INTO t (into) VALUES (1);",
            false },

        // ---------------- Other statements are unaffected ----------------

        { "SELECT still parses",
            "SELECT name FROM employees WHERE salary > 50000;",
            true },

        { "SELECT with JOIN still parses",
            "SELECT * FROM users INNER JOIN orders ON users.id = orders.user_id;",
            true },

        { "CREATE TABLE still parses",
            "CREATE TABLE users (id INTEGER NOT NULL, name STRING);",
            true },

        { "DROP TABLE still parses",
            "DROP TABLE users;",
            true },

        // NULL is accepted as an INSERT value only. WHERE comparisons
        // still do not take it.
        { "NULL is still rejected in WHERE",
            "SELECT * FROM users WHERE name = NULL;",
            false },
    };

    int passed = 0;

    hegadb::client::SQLParser parser;

    for (auto const& test : tests) {

        auto const result = parser.parse(test.sql);

        bool const testPassed = (result.success == test.shouldPass);

        if (testPassed) {
            ++passed;

            std::cout << "[PASS] " << test.name << '\n';
        } else {
            std::cout << "[FAIL] " << test.name << '\n'
                      << "  SQL: " << test.sql << '\n'
                      << "  Expected: "
                      << (test.shouldPass ? "valid" : "invalid") << '\n'
                      << "  Actual: "
                      << (result.success ? "valid" : "invalid") << '\n';

            for (auto const& error : result.errors) {
                std::cout << "  Error: " << error << '\n';
            }
        }
    }

    std::cout
        << "\n========================================\n"
        << " Result: " << passed << "/" << tests.size()
        << " INSERT grammar tests passed\n"
        << "========================================\n";

    return passed == static_cast<int>(tests.size()) ? 0 : 1;
}
