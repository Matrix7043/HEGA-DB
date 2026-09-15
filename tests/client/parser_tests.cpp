#include <hegadb/client/SQLParser.h>

#include <iostream>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::string sql;
    bool shouldPass;
};

int main()
{
    std::vector<TestCase> const tests = {
        { "Basic SELECT *", "SELECT * FROM employees;", true },

        { "Single column", "SELECT name FROM employees;", true },

        { "Multiple columns",
            "SELECT name, salary, department_id FROM employees;",
            true },

        { "WHERE comparison",
            "SELECT name FROM employees WHERE salary > 50000;",
            true },

        { "WHERE string",
            "SELECT name FROM employees WHERE name = 'Alice';",
            true },

        { "WHERE AND",
            "SELECT name FROM employees "
            "WHERE salary > 50000 AND department_id = 10;",
            true },

        { "WHERE OR",
            "SELECT name FROM employees "
            "WHERE salary > 50000 OR department_id = 10;",
            true },

        { "WHERE NOT",
            "SELECT name FROM employees "
            "WHERE NOT salary > 50000;",
            true },

        { "WHERE parentheses",
            "SELECT name FROM employees "
            "WHERE (salary > 50000 OR department_id = 10) "
            "AND active = TRUE;",
            true },

        { "Missing SELECT list",
            "SELECT FROM employees;",
            false },

        { "Missing FROM",
            "SELECT name;",
            false },

        { "Missing table",
            "SELECT name FROM;",
            false },

        { "Missing WHERE expression",
            "SELECT name FROM employees WHERE;",
            false },

        { "Unsupported ORDER BY",
            "SELECT name FROM employees ORDER BY name;",
            false },

        { "Unsupported arithmetic",
            "SELECT salary + 100 FROM employees;",
            false },

        { "Unsupported JOIN",
            "SELECT * FROM employees JOIN departments;",
            false }
    };

    hegadb::client::SQLParser parser;

    int passed = 0;

    std::cout
        << "========================================\n"
        << " HEGADB Parser Tests v0.5\n"
        << "========================================\n\n";

    for (auto const& test : tests) {
        auto const result = parser.parse(test.sql);

        bool const testPassed = result.success == test.shouldPass;

        if (testPassed) {
            ++passed;

            std::cout
                << "[PASS] "
                << test.name
                << '\n';
        } else {
            std::cout
                << "[FAIL] "
                << test.name
                << '\n';

            std::cout
                << "  SQL: "
                << test.sql
                << '\n';

            std::cout
                << "  Expected: "
                << (test.shouldPass ? "valid" : "invalid")
                << '\n';

            std::cout
                << "  Actual: "
                << (result.success ? "valid" : "invalid")
                << '\n';

            for (auto const& error : result.errors) {
                std::cout
                    << "  Error: "
                    << error
                    << '\n';
            }
        }
    }

    std::cout
        << "\n========================================\n"
        << " Result: "
        << passed
        << "/"
        << tests.size()
        << " tests passed\n"
        << "========================================\n";

    return passed == static_cast<int>(tests.size())
        ? 0
        : 1;
}
