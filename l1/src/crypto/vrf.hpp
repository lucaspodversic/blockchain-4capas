// =============================================================================
// vrf.hpp — VRF (Verifiable Random Function) para selección de proposers
// Construcción: proof = Ed25519(seed||slot), output = keccak(proof)
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"

namespace l1::crypto {

struct VrfOutput {
    core::Hash256 random{};
    core::Signature proof{};
};

VrfOutput vrf_prove(const core::PrivateKey& sk, const core::Bytes& alpha);
bool vrf_verify(const core::PublicKey& pk, const core::Bytes& alpha, const VrfOutput& out);
/// Convierte el random VRF a un valor en [0, max) para sorteos.
uint64_t vrf_to_uint64(const core::Hash256& random);

}  // namespace l1::crypto
