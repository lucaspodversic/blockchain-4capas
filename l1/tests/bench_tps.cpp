// =============================================================================
// bench_tps.cpp — Benchmark de transacciones por segundo (aplicación de estado)
// =============================================================================
#include "../src/state/state_db.hpp"
#include "../src/state/state_transition.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    auto params = l1::core::ChainParams::devnet();
    const int N = 2000;
    std::vector<l1::crypto::Ed25519KeyPair> keys;
    std::vector<l1::core::Address> addrs;
    keys.reserve(N + 1);
    for (int i = 0; i < N + 1; ++i) {
        keys.push_back(l1::crypto::ed25519_generate());
        addrs.push_back(l1::crypto::address_from_ed25519(keys.back().public_key));
    }
    l1::state::StateDB db;
    std::map<l1::core::Address, l1::core::Amount> alloc;
    alloc[addrs[0]] = 100'000'000'000ULL;
    db.apply_alloc(alloc);

    l1::state::StateTransition st(db, params, nullptr);
    auto t0 = std::chrono::steady_clock::now();
    int ok = 0;
    for (int i = 0; i < N; ++i) {
        // Simplificado: transferencias desde addrs[0] requieren nonces consecutivos.
        l1::core::Transaction tx;
        tx.type = l1::core::TxType::Transfer;
        tx.from = addrs[0];
        tx.to = addrs[1 + (i % N)];
        tx.nonce = static_cast<uint64_t>(i);
        tx.value = 1;
        tx.gas_limit = 21000;
        tx.max_fee_per_gas = params.initial_base_fee;
        tx.chain_id = params.chain_id;
        tx.signature = l1::crypto::ed25519_sign(keys[0].private_key, tx.signing_hash());
        tx.has_signature = true;
        auto r = st.apply(tx, params.initial_base_fee);
        if (r) ++ok;
    }
    auto t1 = std::chrono::steady_clock::now();
    double sec = std::chrono::duration<double>(t1 - t0).count();
    double tps = ok / sec;
    std::cout << "applied=" << ok << " time_s=" << sec << " tps=" << tps << "\n";
    return ok > 0 ? 0 : 1;
}
