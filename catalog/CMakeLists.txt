add_library(hegadb_catalog
    src/CatalogSnapshot.cpp
    src/InMemoryCatalog.cpp
    src/TableSchema.cpp
)

target_include_directories(hegadb_catalog
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_catalog
    PUBLIC
        hegadb_types
)
