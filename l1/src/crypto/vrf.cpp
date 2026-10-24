// =============================================================================
// vrf.cpp — VRF simple basado en Ed25519 + keccak
// =============================================================================
#include "vrf.hpp"
#include "ed25519.hpp"
#include "hash.hpp"

namespace l1::crypto {

VrfOutput vrf_prove(const core::PrivateKey& sk, const core::Bytes& alpha) {
    VrfOutput out;
    out.proof = ed25519_sign(sk, alpha);
    // Salida aleatoria = hash de la prueba (determinista y verificable).
    out.random = keccak256(out.proof.data(), out.proof.size());
    return out;
}

bool vrf_verify(const core::PublicKey& pk, const core::Bytes& alpha, const VrfOutput& out) {
    if (!ed25519_verify(pk, alpha, out.proof)) return false;
    auto expected = keccak256(out.proof.data(), out.proof.size());
    return expected == out.random;
}

uint64_t vrf_to_uint64(const core::Hash256& random) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 8) | random[i];
    return v;
}

}  // namespace l1::crypto
