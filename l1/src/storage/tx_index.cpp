// =============================================================================
// tx_index.cpp — Índice tx_hash → (block_hash, index)
// =============================================================================
#include "tx_index.hpp"
#include "../core/serialize.hpp"

namespace l1::storage {

TxIndex::TxIndex(std::shared_ptr<KvStore> kv) : kv_(std::move(kv)) {}

core::Result<void> TxIndex::index_tx(const core::Hash256& tx_hash,
                                     const core::Hash256& block_hash, uint32_t index) {
    core::Bytes key{'t', 'x', '/'};
    key.insert(key.end(), tx_hash.begin(), tx_hash.end());
    core::Bytes val;
    core::Encoder e(val);
    e.fixed(block_hash);
    e.u32(index);
    return kv_->put(key, val);
}

core::Result<std::optional<TxLocation>> TxIndex::find(const core::Hash256& tx_hash) const {
    core::Bytes key{'t', 'x', '/'};
    key.insert(key.end(), tx_hash.begin(), tx_hash.end());
    auto raw = kv_->get(key);
    if (!raw) return raw.error();
    if (!raw.value()) return std::optional<TxLocation>{};
    core::Decoder d(*raw.value());
    auto bh = d.fixed<32>(); if (!bh) return bh.error();
    auto idx = d.u32(); if (!idx) return idx.error();
    return std::optional<TxLocation>(TxLocation{bh.value(), idx.value()});
}

}  // namespace l1::storage
