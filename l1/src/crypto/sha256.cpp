// =============================================================================
// sha256.cpp — SHA-256 vía OpenSSL EVP
// =============================================================================
#include "hash.hpp"
#include <openssl/evp.h>
#include <stdexcept>

namespace l1::crypto {

core::Hash256 sha256(const uint8_t* data, size_t len) {
    core::Hash256 out{};
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, data, len) != 1 ||
        EVP_DigestFinal_ex(ctx, out.data(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("SHA-256 failed");
    }
    EVP_MD_CTX_free(ctx);
    return out;
}

}  // namespace l1::crypto
