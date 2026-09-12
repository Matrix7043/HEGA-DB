#include "SQLParser.h"
#include "preparation/LiteralExtractor.h"

#include <cassert>
#include <iostream>

int main()
{

    SQLParser parser;
    hegadb::LiteralExtractor extractor;

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto literals = extractor.extract(*ast);

        assert(literals.size() == 1);
        assert(literals[0].parameter_index == 0);
        assert(literals[0].literal != nullptr);
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
        assert(literals[0].literal != nullptr);

        assert(literals[1].parameter_index == 1);
        assert(literals[1].literal != nullptr);
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
        assert(literals[0].literal != nullptr);
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
