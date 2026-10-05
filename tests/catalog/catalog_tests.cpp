#include <hegadb/catalog/CatalogManager.h>
#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
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
    // ============================================================
    // Initial STUDENTS table
    // ============================================================

    std::vector<hegadb::catalog::ColumnSchema> studentColumns = {
        { "NAME", hegadb::types::DataType::String, false, 0 },
        { "CLASS", hegadb::types::DataType::Integer, false, 1 },
        { "GRADE", hegadb::types::DataType::Integer, false, 2 }
    };

    hegadb::catalog::TableSchema students(
        "STUDENTS",
        studentColumns);

    std::unordered_map<
        std::string,
        hegadb::catalog::TableSchema>
        tables;

    tables.emplace(
        "STUDENTS",
        std::move(students));

    // ============================================================
    // Initial snapshot
    // ============================================================

    auto snapshot = std::make_shared<
        hegadb::catalog::CatalogSnapshot const>(
        1,
        std::move(tables));

    // ============================================================
    // CatalogManager initialization
    // ============================================================

    hegadb::catalog::CatalogManager manager(snapshot);

    assert(manager.version() == 1);

    // ============================================================
    // Existing table lookup
    // ============================================================

    auto const* table = manager.findTable("STUDENTS");

    assert(table != nullptr);
    assert(table->name() == "STUDENTS");

    // ============================================================
    // Existing column lookup
    // ============================================================

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

    // ============================================================
    // Missing column
    // ============================================================

    auto const* missingColumn = table->findColumn("DOES_NOT_EXIST");

    assert(missingColumn == nullptr);

    // ============================================================
    // Missing table
    // ============================================================

    auto const* missingTable = manager.findTable("DOES_NOT_EXIST");

    assert(missingTable == nullptr);

    // ============================================================
    // CREATE TABLE
    // ============================================================

    std::vector<hegadb::catalog::ColumnSchema> courseColumns = {
        { "ID", hegadb::types::DataType::Integer, false, 0 },
        { "TITLE", hegadb::types::DataType::String, false, 1 }
    };

    hegadb::catalog::TableSchema courses(
        "COURSES",
        courseColumns);

    manager.createTable(std::move(courses));

    // Version should increase.
    assert(manager.version() == 2);

    // New table should exist.
    auto const* courseTable = manager.findTable("COURSES");

    assert(courseTable != nullptr);
    assert(courseTable->name() == "COURSES");

    // Existing table must still exist.
    assert(manager.findTable("STUDENTS") != nullptr);

    // ============================================================
    // CREATE TABLE - duplicate table
    // ============================================================

    std::vector<hegadb::catalog::ColumnSchema> duplicateColumns = {
        { "ID", hegadb::types::DataType::Integer, false, 0 }
    };

    hegadb::catalog::TableSchema duplicateStudents(
        "STUDENTS",
        duplicateColumns);

    bool duplicateCreateFailed = false;

    try {
        manager.createTable(std::move(duplicateStudents));
    } catch (std::runtime_error const&) {
        duplicateCreateFailed = true;
    }

    assert(duplicateCreateFailed);

    // Version must NOT change after failed operation.
    assert(manager.version() == 2);

    // Original table must remain intact.
    auto const* studentsTable = manager.findTable("STUDENTS");

    assert(studentsTable != nullptr);
    assert(studentsTable->findColumn("NAME") != nullptr);
    assert(studentsTable->findColumn("CLASS") != nullptr);
    assert(studentsTable->findColumn("GRADE") != nullptr);

    // ============================================================
    // DROP TABLE
    // ============================================================

    manager.dropTable("COURSES");

    // Version should increase again.
    assert(manager.version() == 3);

    // COURSES should no longer exist.
    assert(manager.findTable("COURSES") == nullptr);

    // STUDENTS should still exist.
    assert(manager.findTable("STUDENTS") != nullptr);

    // ============================================================
    // DROP TABLE - missing table
    // ============================================================

    bool missingDropFailed = false;

    try {
        manager.dropTable("DOES_NOT_EXIST");
    } catch (std::runtime_error const&) {
        missingDropFailed = true;
    }

    assert(missingDropFailed);

    // Version must NOT change after failed operation.
    assert(manager.version() == 3);

    // ============================================================
    // Snapshot immutability
    // ============================================================

    std::vector<hegadb::catalog::ColumnSchema> snapshotColumns = {
        { "ID", hegadb::types::DataType::Integer, false, 0 }
    };

    hegadb::catalog::TableSchema snapshotStudents(
        "STUDENTS",
        snapshotColumns);

    std::unordered_map<
        std::string,
        hegadb::catalog::TableSchema>
        snapshotTables;

    snapshotTables.emplace(
        "STUDENTS",
        std::move(snapshotStudents));

    auto originalSnapshot = std::make_shared<
        hegadb::catalog::CatalogSnapshot const>(
        10,
        std::move(snapshotTables));

    hegadb::catalog::CatalogManager snapshotManager(
        originalSnapshot);

    // Verify V10 initially.
    assert(originalSnapshot->version() == 10);
    assert(originalSnapshot->findTable("STUDENTS") != nullptr);
    assert(originalSnapshot->findTable("COURSES") == nullptr);

    // Create a new table through the manager.
    std::vector<hegadb::catalog::ColumnSchema> examColumns = {
        { "ID", hegadb::types::DataType::Integer, false, 0 }
    };

    hegadb::catalog::TableSchema exams(
        "EXAMS",
        examColumns);

    snapshotManager.createTable(std::move(exams));

    // Manager must now be at V11.
    assert(snapshotManager.version() == 11);

    // V11 contains both tables.
    assert(snapshotManager.findTable("STUDENTS") != nullptr);
    assert(snapshotManager.findTable("EXAMS") != nullptr);

    // ============================================================
    // IMPORTANT:
    // The original V10 snapshot must remain unchanged.
    // ============================================================

    assert(originalSnapshot->version() == 10);

    assert(originalSnapshot->findTable("STUDENTS") != nullptr);

    assert(originalSnapshot->findTable("EXAMS") == nullptr);
    // ============================================================
    // Final validation
    // ============================================================

    assert(manager.findTable("STUDENTS") != nullptr);
    assert(manager.findTable("COURSES") == nullptr);
    assert(manager.findTable("DOES_NOT_EXIST") == nullptr);

    std::cout << "All catalog manager tests passed.\n";

    return 0;
}
