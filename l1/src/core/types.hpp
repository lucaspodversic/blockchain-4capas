// =============================================================================
// types.hpp — Alias y tipos base de la Layer 1
// Encaja en: núcleo compartido por todas las capas del nodo C++20.
// =============================================================================
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace l1::core {

/// Hash de 256 bits (32 bytes). Se usa para bloques, txs, state roots, etc.
using Hash256 = std::array<uint8_t, 32>;

/// Dirección de cuenta: 20 bytes (estilo Ethereum, derivados de la clave pública).
using Address = std::array<uint8_t, 20>;

/// Buffer de bytes genérico.
using Bytes = std::vector<uint8_t>;

/// Cantidad en la unidad mínima (wei-like). uint64 alcanza para testnets;
/// producción podría usar uint256; documentado en chain_params.
using Amount = uint64_t;

/// Altura de bloque (número secuencial desde génesis = 0).
using BlockHeight = uint64_t;

/// Nonce de cuenta / slot / época.
using Nonce = uint64_t;
using Slot = uint64_t;
using Epoch = uint64_t;
using Gas = uint64_t;

/// Identificador de cadena (evita replay entre redes).
using ChainId = uint64_t;

/// Clave pública Ed25519 (32 bytes).
using PublicKey = std::array<uint8_t, 32>;

/// Clave privada Ed25519 (32 bytes de seed).
using PrivateKey = std::array<uint8_t, 32>;

/// Firma Ed25519 (64 bytes).
using Signature = std::array<uint8_t, 64>;

/// Hash nulo (todos ceros) — útil como sentinela.
inline Hash256 zero_hash() {
    Hash256 h{};
    h.fill(0);
    return h;
}

/// Dirección nula (todos ceros).
inline Address zero_address() {
    Address a{};
    a.fill(0);
    return a;
}

/// Convierte bytes a hex lowercase con prefijo 0x opcional.
inline std::string to_hex(const uint8_t* data, size_t len, bool prefix = true) {
    std::ostringstream oss;
    if (prefix) oss << "0x";
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        oss << std::setw(2) << static_cast<unsigned>(data[i]);
    }
    return oss.str();
}

template <size_t N>
inline std::string to_hex(const std::array<uint8_t, N>& arr, bool prefix = true) {
    return to_hex(arr.data(), N, prefix);
}

inline std::string to_hex(const Bytes& b, bool prefix = true) {
    return to_hex(b.data(), b.size(), prefix);
}

/// Parsea hex (con o sin 0x) a Bytes. Devuelve vacío si es inválido.
inline Bytes from_hex(std::string_view hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.remove_prefix(2);
    }
    if (hex.size() % 2 != 0) return {};
    Bytes out;
    out.reserve(hex.size() / 2);
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (size_t i = 0; i < hex.size(); i += 2) {
        int hi = nibble(hex[i]);
        int lo = nibble(hex[i + 1]);
        if (hi < 0 || lo < 0) return {};
        out.push_back(static_cast<uint8_t>((hi << 4) | lo));
    }
    return out;
}

template <size_t N>
inline std::optional<std::array<uint8_t, N>> from_hex_arr(std::string_view hex) {
    auto b = from_hex(hex);
    if (b.size() != N) return std::nullopt;
    std::array<uint8_t, N> out{};
    std::memcpy(out.data(), b.data(), N);
    return out;
}

}  // namespace l1::core
