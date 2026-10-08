# ============================================================
# DB Server Catalog
# ============================================================

add_library(hegadb_catalog_server
    src/db-server/CatalogManager.cpp
    src/CatalogSnapshot.cpp
    src/TableSchema.cpp
)

target_include_directories(hegadb_catalog_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_catalog_server
    PUBLIC
        hegadb_types
    PRIVATE
        hegadb_warnings
)


# ============================================================
# DB Client Catalog
# ============================================================

add_library(hegadb_catalog_client
    src/db-client/InMemoryCatalog.cpp
    src/CatalogAdapter.cpp
    src/CatalogSnapshot.cpp
    src/TableSchema.cpp
)

target_include_directories(hegadb_catalog_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_catalog_client
    PUBLIC
        hegadb_catalog_client_transport
        hegadb_types
    PRIVATE
        hegadb_warnings
)


# ============================================================
# Catalog Service
# ============================================================

# add_library(hegadb_catalog_service
# )
#
# target_include_directories(hegadb_catalog_service
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
#
# target_link_libraries(hegadb_catalog_service
#     PUBLIC
#         hegadb_catalog_server_transport
#     PRIVATE
#         hegadb_warnings
# )

