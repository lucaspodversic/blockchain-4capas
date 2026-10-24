// =============================================================================
// bls.hpp — Firmas BLS y agregación (votos de finalidad)
// Fallback: esquema didáctico basado en Ed25519 + XOR de firmas para tests.
// Interfaz compatible con un swap-in de blst/herumi (documentado en README).
// NO es BLS12-381 criptográficamente seguro; sirve para ejercitar el gadget.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <vector>

namespace l1::crypto {

using BlsPublicKey = core::PublicKey;   // 32B placeholder
using BlsSecretKey = core::PrivateKey;
using BlsSignature = core::Signature;   // 64B placeholder

struct BlsKeyPair {
    BlsPublicKey public_key{};
    BlsSecretKey secret_key{};
};

BlsKeyPair bls_generate();
BlsSignature bls_sign(const BlsSecretKey& sk, const core::Hash256& msg);
bool bls_verify(const BlsPublicKey& pk, const core::Hash256& msg, const BlsSignature& sig);
/// Agrega firmas (XOR byte a byte — placeholder). Producción: pairing aggregate.
BlsSignature bls_aggregate(const std::vector<BlsSignature>& sigs);
bool bls_fast_aggregate_verify(const std::vector<BlsPublicKey>& pks,
                               const core::Hash256& msg,
                               const BlsSignature& agg);

}  // namespace l1::crypto
