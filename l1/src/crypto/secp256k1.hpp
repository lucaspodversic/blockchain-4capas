// =============================================================================
// secp256k1.hpp — Firmas ECDSA secp256k1 (compatibilidad con wallets Eth-like)
// Fallback: OpenSSL EC_KEY; si se quiere libsecp256k1 nativa, sustituir el .cpp.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"

namespace l1::crypto {

struct SecpKeyPair {
    core::Bytes public_key;   // no-comprimida 65 bytes (0x04||X||Y) o comprimida 33
    core::Bytes private_key;  // 32 bytes
};

SecpKeyPair secp256k1_generate();
core::Result<core::Bytes> secp256k1_sign(const core::Bytes& priv32, const core::Hash256& digest);
bool secp256k1_verify(const core::Bytes& pub, const core::Hash256& digest, const core::Bytes& sig_der);
/// Recupera dirección Eth-like (keccak(pubkey[1:])[12:]).
core::Address secp256k1_address(const core::Bytes& uncompressed_pub65);

}  // namespace l1::crypto
