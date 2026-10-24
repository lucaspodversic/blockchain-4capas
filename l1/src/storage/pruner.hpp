// =============================================================================
// pruner.hpp — API de poda histórica
// =============================================================================
#pragma once
#include "kv_store.hpp"
#include "block_store.hpp"
#include <memory>

namespace l1::storage {

class Pruner {
public:
    Pruner(std::shared_ptr<KvStore> kv, BlockStore& blocks);
    core::Result<uint64_t> prune_blocks_before(core::BlockHeight tip, uint64_t retain);
private:
    std::shared_ptr<KvStore> kv_;
    BlockStore& blocks_;
};

}  // namespace l1::storage
