#include<hegadb/crypto/KeyManager.h>
#include<hegadb/crypto/OpenFHEContext.h>

#include<iostream>
#include<cassert>

int main(){
    std::cout
    << "========================================\n"
    << " HEGADB Crypto Context Tests\n"
    << "========================================\n\n";

    // ========================================
    // Test 1: OpenFHE context initialization
    // ========================================

    std::cout << "[TEST 1] OpenFHEContext initialization...\n";
    {
        hegadb::crypto::OpenFHEContext context;

        auto bfvContext = context.getBFVContext();
        auto ckksContext = context.getCKKSContext();

        assert(bfvContext != nullptr);
        assert(ckksContext != nullptr);

        std::cout<<"  PASS: Both BFV and CKKS contexts initialized\n";
    }


    // ========================================
    // Test 2: KeyManager initialization
    // ========================================

    std::cout << "[TEST 2] KeyManager initialization...\n";

    {
        hegadb::crypto::OpenFHEContext context;
        hegadb::crypto::KeyManager keymanager(context);

        assert(!keymanager.hasKeys());
        std::cout << "  PASS: KeyManager created without keys\n";

    }


    // ========================================
    // Test 3: Key generation
    // ========================================

    std::cout << "[TEST 3] Key generation...\n";

    {
        hegadb::crypto::OpenFHEContext context;
        hegadb::crypto::KeyManager keyManager(context);

        keyManager.generateKeys();

        assert(keyManager.hasKeys());

        std::cout << "  PASS: Keys generated successfully\n";
    }

    // ========================================
    // Test 4: Key access
    // ========================================

    std::cout << "[TEST 4] Key access...\n";

    {
        hegadb::crypto::OpenFHEContext context;
        hegadb::crypto::KeyManager keyManager(context);

        keyManager.generateKeys();

        auto bfvPublic = keyManager.getBFVPublicKey();
        auto bfvPrivate = keyManager.getBFVPrivateKey();
        auto ckksPublic = keyManager.getCKKSPublicKey();
        auto ckksPrivate = keyManager.getCKKSPrivateKey();

        assert(bfvPublic != nullptr);
        assert(bfvPrivate != nullptr);
        assert(ckksPublic != nullptr);
        assert(ckksPrivate != nullptr);

        std::cout << "  PASS: All keys accessible\n";
    }

    // ========================================
    // Test 5: Evaluation key generation
    // ========================================

    std::cout << "[TEST 5] Evaluation key generation...\n";

    {
        hegadb::crypto::OpenFHEContext context;
        hegadb::crypto::KeyManager keyManager(context);

        keyManager.generateKeys();
        keyManager.generateEvaluationKeys();

        std::cout << "  PASS: Evaluation keys generated\n";
    }

    // ========================================
    // Test 6: Multiple context creation
    // ========================================

    std::cout << "[TEST 6] Multiple context creation...\n";

    {
        hegadb::crypto::OpenFHEContext context1;
        hegadb::crypto::OpenFHEContext context2;

        auto bfv1 = context1.getBFVContext();
        auto bfv2 = context2.getBFVContext();

        assert(bfv1 != nullptr);
        assert(bfv2 != nullptr);

        std::cout << "  PASS: Multiple contexts created independently\n";
    }

    std::cout
        << "\n========================================\n"
        << " All context tests passed! (6/6)\n"
        << "========================================\n";

    return 0;    
    
}