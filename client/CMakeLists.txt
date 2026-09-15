find_package(antlr4-runtime CONFIG REQUIRED)

set(ANTLR4_RUNTIME_INCLUDE_DIR
    "/usr/include/antlr4-runtime"
)

add_library(hegadb_client_core
    src/SQLParser.cpp
    src/SyntaxErrorListener.cpp

    src/ast/ASTBuilder.cpp
    src/ast/ASTParameterizer.cpp
    src/ast/ASTPrinter.cpp

    src/preparation/EncryptedParameter.cpp
    src/preparation/LiteralExtractor.cpp
    src/preparation/PreparedQuery.cpp
    src/preparation/QueryPreparer.cpp

    src/semantic/SemanticValidator.cpp

    ${PROJECT_SOURCE_DIR}/generated/HEGADBLexer.cpp
    ${PROJECT_SOURCE_DIR}/generated/HEGADBParser.cpp
    ${PROJECT_SOURCE_DIR}/generated/HEGADBParserBaseListener.cpp
    ${PROJECT_SOURCE_DIR}/generated/HEGADBParserBaseVisitor.cpp
    ${PROJECT_SOURCE_DIR}/generated/HEGADBParserListener.cpp
    ${PROJECT_SOURCE_DIR}/generated/HEGADBParserVisitor.cpp
)

target_include_directories(hegadb_client_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/generated

    PRIVATE
        ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

target_link_libraries(hegadb_client_core
    PUBLIC
        hegadb_operations
        antlr4-runtime
)

add_executable(hegadb_client
    src/main.cpp
)

target_link_libraries(hegadb_client
    PRIVATE
        hegadb_client_core
)
