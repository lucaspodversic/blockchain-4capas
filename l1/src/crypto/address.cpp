#include <cstring>
// =============================================================================
// address.cpp — Derivación y validación de direcciones (checksum estilo EIP-55)
// Encaja en: wallets, RPC y génesis.
// =============================================================================
#include "address.hpp"
#include "../core/types.hpp"
#include "../core/error.hpp"
#include "hash.hpp"
#include "ed25519.hpp"
#include <cctype>

namespace l1::crypto {

/// Dirección a partir de pubkey Ed25519: keccak(pubkey)[12:32].
core::Address address_from_ed25519(const core::PublicKey& pk) {
    auto h = keccak256(pk.data(), pk.size());
    core::Address a{};
    std::memcpy(a.data(), h.data() + 12, 20);
    return a;
}

/// Checksum EIP-55: mezcla mayúsculas según keccak del hex lowercase.
std::string address_to_checksum(const core::Address& addr) {
    auto hex = core::to_hex(addr, false);  // 40 chars lowercase
    auto hash = keccak256(reinterpret_cast<const uint8_t*>(hex.data()), hex.size());
    std::string out = "0x";
    for (size_t i = 0; i < 40; ++i) {
        char c = hex[i];
        // Nibble alto del byte hash[i/2]
        uint8_t nibble = (hash[i / 2] >> (i % 2 == 0 ? 4 : 0)) & 0x0f;
        if (c >= 'a' && c <= 'f' && nibble >= 8) c = static_cast<char>(c - 32);
        out.push_back(c);
    }
    return out;
}

bool address_is_valid_hex(std::string_view s) {
    auto a = core::from_hex_arr<20>(s);
    return a.has_value();
}

}  // namespace l1::crypto
