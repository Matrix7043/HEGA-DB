#include "SQLParser.h"
#include "crypto/MockCiphertext.h"
#include "crypto/MockEncryption.h"
#include "preparation/QueryPreparer.h"

#include <cassert>
#include <iostream>
#include <variant>

int main()
{
    SQLParser parser;
    hegadb::MockEncryption encryption;
    hegadb::QueryPreparer preparer(encryption);

    // Single literal
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS WHERE GRADE > 90;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        // Query should be preserved.
        assert(prepared.query().kind() == hegadb::ASTNodeKind::SelectStmt);

        // One encrypted parameter.
        assert(prepared.parameters().size() == 1);
        assert(prepared.parameters()[0].index() == 0);

        // Verify the mock ciphertext contains the expected plaintext.
        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(&ciphertext);

        assert(mockCiphertext != nullptr);

        auto const& value = mockCiphertext->value().value();

        assert(std::holds_alternative<long long>(value));
        assert(std::get<long long>(value) == 90);
    }

    // Multiple literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE GRADE > 90 AND CLASS = 10;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        assert(prepared.parameters().size() == 2);

        // First parameter
        assert(prepared.parameters()[0].index() == 0);

        {
            auto const& ciphertext = prepared.parameters()[0].ciphertext();

            auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
                &ciphertext);

            assert(mockCiphertext != nullptr);

            auto const& value = mockCiphertext->value().value();

            assert(std::holds_alternative<long long>(value));
            assert(std::get<long long>(value) == 90);
        }

        // Second parameter
        assert(prepared.parameters()[1].index() == 1);

        {
            auto const& ciphertext = prepared.parameters()[1].ciphertext();

            auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
                &ciphertext);

            assert(mockCiphertext != nullptr);

            auto const& value = mockCiphertext->value().value();

            assert(std::holds_alternative<long long>(value));
            assert(std::get<long long>(value) == 10);
        }
    }

    // NOT expression
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS "
            "WHERE NOT (GRADE > 90);");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        assert(prepared.parameters().size() == 1);
        assert(prepared.parameters()[0].index() == 0);

        auto const& ciphertext = prepared.parameters()[0].ciphertext();

        auto const* mockCiphertext = dynamic_cast<hegadb::MockCiphertext const*>(
            &ciphertext);

        assert(mockCiphertext != nullptr);

        auto const& value = mockCiphertext->value().value();

        assert(std::holds_alternative<long long>(value));
        assert(std::get<long long>(value) == 90);
    }

    // No literals
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM STUDENTS;");

        assert(ast != nullptr);

        auto prepared = preparer.prepare(std::move(ast));

        assert(prepared.parameters().empty());

        // Query must still exist.
        assert(prepared.query().kind() == hegadb::ASTNodeKind::SelectStmt);
    }

    std::cout << "Query preparer tests passed.\n";

    return 0;
}
