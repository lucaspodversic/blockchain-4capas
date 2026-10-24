// =============================================================================
// wallet_cli.cpp — Wallet CLI: crear clave, firmar, enviar
// =============================================================================
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"
#include "../src/crypto/keystore.hpp"
#include "../src/core/transaction.hpp"
#include "../src/crypto/hash.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "uso: wallet_cli new|addr|sign|keystore <args>\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "new") {
        auto kp = l1::crypto::ed25519_generate();
        auto addr = l1::crypto::address_from_ed25519(kp.public_key);
        std::cout << "address=" << l1::core::to_hex(addr) << "\n";
        std::cout << "priv=" << l1::core::to_hex(kp.private_key) << "\n";
        std::cout << "pub=" << l1::core::to_hex(kp.public_key) << "\n";
        return 0;
    }
    if (cmd == "keystore" && argc >= 4) {
        auto kp = l1::crypto::ed25519_generate();
        auto r = l1::crypto::keystore_save_file(argv[2], kp.private_key, argv[3]);
        if (!r) { std::cerr << r.error().to_string() << "\n"; return 1; }
        auto addr = l1::crypto::address_from_ed25519(kp.public_key);
        std::cout << "saved " << argv[2] << " address=" << l1::core::to_hex(addr) << "\n";
        return 0;
    }
    if (cmd == "sign" && argc >= 4) {
        auto sk = l1::core::from_hex_arr<32>(argv[2]);
        if (!sk) { std::cerr << "bad key\n"; return 1; }
        auto msg = l1::core::from_hex(argv[3]);
        auto sig = l1::crypto::ed25519_sign(*sk, msg);
        std::cout << l1::core::to_hex(sig) << "\n";
        return 0;
    }
    std::cerr << "comando desconocido\n";
    return 1;
}
