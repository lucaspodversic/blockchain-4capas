// =============================================================================
// block_store.hpp — API de almacenamiento de bloques
// =============================================================================
#pragma once
#include "kv_store.hpp"
#include "../core/block.hpp"
#include <memory>

namespace l1::storage {

class BlockStore {
public:
    explicit BlockStore(std::shared_ptr<KvStore> kv);
    core::Result<void> put_block(const core::Block& block);
    core::Result<std::optional<core::Block>> get_by_hash(const core::Hash256& h) const;
    core::Result<std::optional<core::Block>> get_by_height(core::BlockHeight h) const;
    core::Result<void> set_canonical_tip(const core::Hash256& h);
    core::Result<std::optional<core::Hash256>> canonical_tip() const;

private:
    static core::Bytes prefix_hash(const core::Hash256& h);
    static core::Bytes prefix_height(core::BlockHeight height);
    std::shared_ptr<KvStore> kv_;
};

}  // namespace l1::storage
