// =============================================================================
// tx_index.hpp — Índice de transacciones
// =============================================================================
#pragma once
#include "kv_store.hpp"
#include <memory>
#include <optional>

namespace l1::storage {

struct TxLocation {
    core::Hash256 block_hash{};
    uint32_t index{0};
};

class TxIndex {
public:
    explicit TxIndex(std::shared_ptr<KvStore> kv);
    core::Result<void> index_tx(const core::Hash256& tx_hash,
                                const core::Hash256& block_hash, uint32_t index);
    core::Result<std::optional<TxLocation>> find(const core::Hash256& tx_hash) const;
private:
    std::shared_ptr<KvStore> kv_;
};

}  // namespace l1::storage
