// =============================================================================
// trie.cpp — Implementación del trie y cálculo de raíz
// =============================================================================
#include "trie.hpp"
#include "../crypto/hash.hpp"

namespace l1::state {

void Trie::put(const core::Bytes& key, const core::Bytes& value) {
    data_[key] = value;
}

void Trie::del(const core::Bytes& key) {
    data_.erase(key);
}

std::optional<core::Bytes> Trie::get(const core::Bytes& key) const {
    auto it = data_.find(key);
    if (it == data_.end()) return std::nullopt;
    return it->second;
}

core::Hash256 Trie::root_hash() const {
    if (data_.empty()) return core::zero_hash();
    // Raíz = keccak de la concatenación ordenada de (keccak(k)||keccak(v)).
    core::Bytes buf;
    for (const auto& [k, v] : data_) {
        auto hk = crypto::keccak256(k);
        auto hv = crypto::keccak256(v);
        buf.insert(buf.end(), hk.begin(), hk.end());
        buf.insert(buf.end(), hv.begin(), hv.end());
    }
    return crypto::keccak256(buf);
}

}  // namespace l1::state
