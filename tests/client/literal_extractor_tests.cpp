#include <hegadb/client/SQLParser.h>
#include <hegadb/client/preparation/LiteralExtractor.h>

#include <cassert>
#include <iostream>

int main()
{
    hegadb::client::SQLParser parser;
    hegadb::client::preparation::LiteralExtractor extractor;

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto literals = extractor.extract(*ast);

        assert(literals.size() == 1);
        assert(literals[0].parameter_index == 0);
        assert(std::get<long long>(literals[0].value) == 90);
    }

    // Multiple literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE GRADE > 90 AND CLASS = 10;");

        assert(ast != nullptr);

        auto literals = extractor.extract(*ast);

        assert(literals.size() == 2);

        assert(literals[0].parameter_index == 0);
        assert(std::get<long long>(literals[0].value) == 90);

        assert(literals[1].parameter_index == 1);
        assert(std::get<long long>(literals[1].value) == 10);
    }

    // NOT expression
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE NOT (GRADE > 90);");

        assert(ast != nullptr);

        auto literals = extractor.extract(*ast);

        assert(literals.size() == 1);
        assert(literals[0].parameter_index == 0);
        assert(std::get<long long>(literals[0].value) == 90);
    }

    // No literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS;");

        assert(ast != nullptr);

        auto literals = extractor.extract(*ast);

        assert(literals.empty());
    }

    std::cout << "Literal extractor tests passed.\n";

    return 0;
}
