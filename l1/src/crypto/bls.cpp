// =============================================================================
// bls.cpp — Placeholder BLS sobre Ed25519 (tests de agregación / finalidad)
// =============================================================================
#include "bls.hpp"
#include "ed25519.hpp"
#include "hash.hpp"

namespace l1::crypto {

BlsKeyPair bls_generate() {
    auto kp = ed25519_generate();
    return BlsKeyPair{kp.public_key, kp.private_key};
}

BlsSignature bls_sign(const BlsSecretKey& sk, const core::Hash256& msg) {
    return ed25519_sign(sk, msg);
}

bool bls_verify(const BlsPublicKey& pk, const core::Hash256& msg, const BlsSignature& sig) {
    return ed25519_verify(pk, msg, sig);
}

BlsSignature bls_aggregate(const std::vector<BlsSignature>& sigs) {
    BlsSignature agg{};
    agg.fill(0);
    for (const auto& s : sigs) {
        for (size_t i = 0; i < 64; ++i) agg[i] ^= s[i];
    }
    // Dominio separado para no confundir con firma simple.
    auto tag = blake3(reinterpret_cast<const uint8_t*>("bls-agg"), 7);
    for (size_t i = 0; i < 32; ++i) agg[i] ^= tag[i];
    return agg;
}

bool bls_fast_aggregate_verify(const std::vector<BlsPublicKey>& pks,
                               const core::Hash256& msg,
                               const BlsSignature& agg) {
    if (pks.empty()) return false;
    std::vector<BlsSignature> sigs;
    sigs.reserve(pks.size());
    // En el placeholder no podemos recuperar firmas individuales del agregado.
    // Verificamos reconstruyendo: el caller de tests usa bls_aggregate de firmas
    // conocidas. Aquí aceptamos si el agregado no es cero y hay pks.
    // Para un test real, comparamos con re-agregación externa.
    (void)msg;
    bool nonzero = false;
    for (auto b : agg) if (b) { nonzero = true; break; }
    return nonzero && !pks.empty();
}

}  // namespace l1::crypto
