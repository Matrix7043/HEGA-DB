#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>

#include <hegadb/types/DataType.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

int main()
{
    std::vector<hegadb::catalog::ColumnSchema> columns = {
        { "NAME", hegadb::types::DataType::String, false, 0 },
        { "CLASS", hegadb::types::DataType::Integer, false, 1 },
        { "GRADE", hegadb::types::DataType::Integer, false, 2 }
    };

    hegadb::catalog::TableSchema students(
        "STUDENTS",
        columns);

    std::unordered_map<
        std::string,
        hegadb::catalog::TableSchema>
        tables;

    tables.emplace(
        "STUDENTS",
        std::move(students));

    auto snapshot = std::make_shared<
        hegadb::catalog::CatalogSnapshot const>(1, std::move(tables));

    hegadb::catalog::InMemoryCatalog catalog(snapshot);

    assert(catalog.version() == 1);

    auto const* table = catalog.findTable("STUDENTS");

    assert(table != nullptr);
    assert(table->name() == "STUDENTS");

    auto const* nameColumn = table->findColumn("NAME");

    assert(nameColumn != nullptr);
    assert(nameColumn->name == "NAME");
    assert(
        nameColumn->type == hegadb::types::DataType::String);

    auto const* classColumn = table->findColumn("CLASS");

    assert(classColumn != nullptr);
    assert(classColumn->name == "CLASS");
    assert(
        classColumn->type == hegadb::types::DataType::Integer);

    auto const* gradeColumn = table->findColumn("GRADE");

    assert(gradeColumn != nullptr);
    assert(gradeColumn->name == "GRADE");
    assert(
        gradeColumn->type == hegadb::types::DataType::Integer);

    auto const* missingColumn = table->findColumn("DOES_NOT_EXIST");

    assert(missingColumn == nullptr);

    auto const* missingTable = catalog.findTable("DOES_NOT_EXIST");

    assert(missingTable == nullptr);

    std::cout << "All catalog tests passed.\n";

    return 0;
}
