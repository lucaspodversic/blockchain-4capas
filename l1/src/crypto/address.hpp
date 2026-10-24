// =============================================================================
// address.hpp — API de direcciones
// =============================================================================
#pragma once
#include "../core/types.hpp"
namespace l1::crypto {
core::Address address_from_ed25519(const core::PublicKey& pk);
std::string address_to_checksum(const core::Address& addr);
bool address_is_valid_hex(std::string_view s);
}  // namespace l1::crypto
