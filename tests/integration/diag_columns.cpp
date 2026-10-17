#include <hegadb/server/storage/StorageManager.h>
#include <hegadb/server/storage/EncryptedTable.h>
#include <hegadb/server/storage/EncryptedBatch.h>
#include <hegadb/crypto/CompositeEncryption.h>
#include <openfhe.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
using namespace hegadb;

int main() {
    std::string home = std::getenv("HOME");
    auto enc = std::make_shared<crypto::CompositeEncryption>(
        home + "/.hegadb/keys");
    auto storage = server::storage::StorageManager::createFileStorage(
        home + "/.hegadb/data",
        home + "/.hegadb/catalog");
    auto table = storage->openTable("EMPLOYEES");
    table->open();
    auto batch = table->nextBatch();
    if (!batch) { std::cerr << "no batch\n"; return 1; }

    auto bfv = enc->getContext().getBFVContext();
    auto priv = enc->getKeyManager().getBFVPrivateKey();

    for (auto const& [name, ct] : batch->columns) {
        lbcrypto::Plaintext pt;
        bfv->Decrypt(priv, ct.getCiphertext(), &pt);
        auto v = pt->GetPackedValue();
        // Fingerprint deep in the polynomial data (offset 50000)
        std::ostringstream ss;
        lbcrypto::Serial::Serialize(ct.getCiphertext(), ss, lbcrypto::SerType::BINARY);
        std::string s = ss.str();
        std::cout << name << " @50000=";
        for (int i=50000; i<50006 && i<(int)s.size(); i++)
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << (unsigned)(unsigned char)s[i];
        std::cout << std::dec << " slots[0..2]=" << v[0] << " " << v[1] << " " << v[2] << "\n";
    }
    return 0;
}
