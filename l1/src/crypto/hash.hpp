// =============================================================================
// hash.hpp — Interfaz unificada de funciones hash (SHA-256, Keccak-256, BLAKE3)
// Encaja en: direcciones, state roots, ids de tx/bloque y usos internos.
//
// Fallback documentado:
//   - SHA-256  → OpenSSL EVP
//   - Keccak-256 → implementación pura (no SHA3-NIST; es el Keccak original)
//   - BLAKE3   → si no hay libblake3, usamos BLAKE2b-256 de libsodium
//                (misma API Hash256; documentado en README)
// =============================================================================
#pragma once

#include "../core/types.hpp"
#include <cstring>

namespace l1::crypto {

core::Hash256 sha256(const uint8_t* data, size_t len);
inline core::Hash256 sha256(const core::Bytes& b) { return sha256(b.data(), b.size()); }

core::Hash256 keccak256(const uint8_t* data, size_t len);
inline core::Hash256 keccak256(const core::Bytes& b) { return keccak256(b.data(), b.size()); }

core::Hash256 blake3(const uint8_t* data, size_t len);
inline core::Hash256 blake3(const core::Bytes& b) { return blake3(b.data(), b.size()); }

/// Hash de dos nodos Merkle (orden lexicográfico no; concat left||right).
inline core::Hash256 hash_pair(const core::Hash256& left, const core::Hash256& right) {
    uint8_t buf[64];
    std::memcpy(buf, left.data(), 32);
    std::memcpy(buf + 32, right.data(), 32);
    return keccak256(buf, 64);
}

}  // namespace l1::crypto
