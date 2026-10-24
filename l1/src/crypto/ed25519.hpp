// =============================================================================
// ed25519.hpp — API de firmas Ed25519 (validadores y txs del nodo)
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <stdexcept>

namespace l1::crypto {

struct Ed25519KeyPair {
    core::PublicKey public_key{};
    core::PrivateKey private_key{};  // seed 32B
};

Ed25519KeyPair ed25519_generate();
core::PublicKey ed25519_derive_public(const core::PrivateKey& seed);
core::Signature ed25519_sign(const core::PrivateKey& seed, const uint8_t* msg, size_t len);
core::Signature ed25519_sign(const core::PrivateKey& seed, const core::Bytes& msg);
core::Signature ed25519_sign(const core::PrivateKey& seed, const core::Hash256& h);
bool ed25519_verify(const core::PublicKey& pk, const uint8_t* msg, size_t len,
                    const core::Signature& sig);
bool ed25519_verify(const core::PublicKey& pk, const core::Bytes& msg, const core::Signature& sig);
bool ed25519_verify(const core::PublicKey& pk, const core::Hash256& h, const core::Signature& sig);

}  // namespace l1::crypto
