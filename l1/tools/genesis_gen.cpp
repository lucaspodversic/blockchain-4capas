// =============================================================================
// genesis_gen.cpp — Generador de génesis para testnets
// =============================================================================
#include "../src/core/genesis.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    std::string out = "genesis.json";
    int nvals = 1;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) out = argv[++i];
        else if (std::string(argv[i]) == "-n" && i + 1 < argc) nvals = std::stoi(argv[++i]);
    }
    l1::core::GenesisConfig g;
    g.params = l1::core::ChainParams::devnet();
    g.timestamp = 1700000000;
    for (int i = 0; i < nvals; ++i) {
        auto kp = l1::crypto::ed25519_generate();
        auto addr = l1::crypto::address_from_ed25519(kp.public_key);
        g.validators.push_back({addr, kp.public_key, g.params.min_validator_stake});
        g.alloc[addr] = 1'000'000'000'000ULL;
        std::cerr << "validator[" << i << "] addr=" << l1::core::to_hex(addr)
                  << " priv=" << l1::core::to_hex(kp.private_key) << "\n";
    }
    std::ofstream f(out);
    f << g.to_json();
    std::cout << "wrote " << out << "\n";
    return 0;
}
