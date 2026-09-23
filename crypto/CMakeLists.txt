# Find OpenFHE package
find_package(OpenFHE REQUIRED)

add_library(hegadb_crypto
    src/MockCiphertext.cpp
    src/MockEncryption.cpp
    src/Plaintext.cpp

    # New OpenFHE implementation files
    src/OpenFHEContext.cpp
    src/OpenFHECiphertext.cpp
    src/KeyManager.cpp
    src/BFVEncryption.cpp
    src/CKKSEncryption.cpp
    src/CompositeEncryption.cpp

)

target_include_directories(hegadb_crypto
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe

)

target_link_libraries(hegadb_crypto
  PUBLIC
    hegadb_types
    OPENFHEpke
    OPENFHEbinfhe
    OPENFHEcore
)

# Set C++ standard required by OpenFHE
set_target_properties(hegadb_crypto PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
