// =============================================================================
// trie.hpp — Merkle Patricia Trie simplificado (mapa hash → valor + raíz keccak)
// Encaja en: state root verificable. No es MPT hex-prefix completo de Ethereum;
// es un mapa ordenado cuya raíz = keccak(concat de keccak(key)||keccak(value)).
// Suficiente para tests y L2 verifier; upgrade path documentado en README.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"
#include <map>
#include <optional>

namespace l1::state {

class Trie {
public:
    void put(const core::Bytes& key, const core::Bytes& value);
    void del(const core::Bytes& key);
    std::optional<core::Bytes> get(const core::Bytes& key) const;
    core::Hash256 root_hash() const;
    bool empty() const { return data_.empty(); }
    const std::map<core::Bytes, core::Bytes>& entries() const { return data_; }

    /// Clona el trie (para snapshots / journal).
    Trie clone() const { return *this; }

private:
    std::map<core::Bytes, core::Bytes> data_;
};

}  // namespace l1::state
