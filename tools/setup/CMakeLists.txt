cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# HEGA-DB Setup Tools Library
# ==============================================================================

# Create setup tools library
add_library(hegadb_setup_tools
    src/EmployeeDataGenerator.cpp
    src/FileStorageWriter.cpp
)

target_include_directories(hegadb_setup_tools
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_setup_tools
    PUBLIC
        hegadb_crypto
        hegadb_catalog_client
        hegadb_server
)

# ==============================================================================
# Database Setup Executable
# ==============================================================================

add_executable(hegadb_setup
    src/database_setup.cpp
)

target_link_libraries(hegadb_setup
    PRIVATE
        hegadb_setup_tools
        hegadb_crypto
        hegadb_catalog_client
        hegadb_server
)

# Install the executable
install(TARGETS hegadb_setup
    RUNTIME DESTINATION bin
)

message(STATUS "HEGA-DB Setup Tools configured")
