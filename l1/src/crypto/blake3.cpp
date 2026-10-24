// =============================================================================
// blake3.cpp — Hash rápido para uso interno
// Fallback: BLAKE2b-256 de libsodium (API idéntica Hash256).
// Para producción con BLAKE3 real: vincular libblake3 y reemplazar el cuerpo.
// =============================================================================
#include "hash.hpp"
#include <sodium.h>
#include <stdexcept>

namespace l1::crypto {

namespace {
struct SodiumInit {
    SodiumInit() {
        if (sodium_init() < 0) throw std::runtime_error("sodium_init failed");
    }
};
static SodiumInit g_sodium;
}  // namespace

core::Hash256 blake3(const uint8_t* data, size_t len) {
    // BLAKE2b-256 como sustituto documentado de BLAKE3.
    core::Hash256 out{};
    if (crypto_generichash_blake2b(out.data(), out.size(), data, len,
                                   nullptr, 0) != 0) {
        throw std::runtime_error("blake2b failed");
    }
    return out;
}

}  // namespace l1::crypto
