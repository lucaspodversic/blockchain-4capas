// =============================================================================
// ed25519.cpp — Firmas Ed25519 vía libsodium
// =============================================================================
#include "ed25519.hpp"
#include <sodium.h>

namespace l1::crypto {

namespace {
void ensure_sodium() {
    if (sodium_init() < 0) throw std::runtime_error("sodium_init failed");
}
}  // namespace

Ed25519KeyPair ed25519_generate() {
    ensure_sodium();
    Ed25519KeyPair kp;
    unsigned char sk[64];
    crypto_sign_ed25519_keypair(kp.public_key.data(), sk);
    crypto_sign_ed25519_sk_to_seed(kp.private_key.data(), sk);
    sodium_memzero(sk, sizeof sk);
    return kp;
}

core::PublicKey ed25519_derive_public(const core::PrivateKey& seed) {
    ensure_sodium();
    unsigned char sk[64];
    core::PublicKey pk{};
    crypto_sign_ed25519_seed_keypair(pk.data(), sk, seed.data());
    sodium_memzero(sk, sizeof sk);
    return pk;
}

core::Signature ed25519_sign(const core::PrivateKey& seed, const uint8_t* msg, size_t len) {
    ensure_sodium();
    unsigned char sk[64];
    core::PublicKey pk{};
    crypto_sign_ed25519_seed_keypair(pk.data(), sk, seed.data());
    core::Signature sig{};
    unsigned long long siglen = 0;
    crypto_sign_ed25519_detached(sig.data(), &siglen, msg, len, sk);
    sodium_memzero(sk, sizeof sk);
    return sig;
}

core::Signature ed25519_sign(const core::PrivateKey& seed, const core::Bytes& msg) {
    return ed25519_sign(seed, msg.data(), msg.size());
}

core::Signature ed25519_sign(const core::PrivateKey& seed, const core::Hash256& h) {
    return ed25519_sign(seed, h.data(), h.size());
}

bool ed25519_verify(const core::PublicKey& pk, const uint8_t* msg, size_t len,
                    const core::Signature& sig) {
    ensure_sodium();
    return crypto_sign_ed25519_verify_detached(sig.data(), msg, len, pk.data()) == 0;
}

bool ed25519_verify(const core::PublicKey& pk, const core::Bytes& msg, const core::Signature& sig) {
    return ed25519_verify(pk, msg.data(), msg.size(), sig);
}

bool ed25519_verify(const core::PublicKey& pk, const core::Hash256& h, const core::Signature& sig) {
    return ed25519_verify(pk, h.data(), h.size(), sig);
}

}  // namespace l1::crypto
