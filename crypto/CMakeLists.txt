add_library(hegadb_crypto
    src/MockCiphertext.cpp
    src/MockEncryption.cpp
    src/Plaintext.cpp
)

target_include_directories(hegadb_crypto
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_crypto
  PUBLIC
    hegadb_types
)
