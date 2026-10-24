// =============================================================================
// pruner.cpp — Poda de datos históricos
// =============================================================================
#include "pruner.hpp"

namespace l1::storage {

Pruner::Pruner(std::shared_ptr<KvStore> kv, BlockStore& blocks)
    : kv_(std::move(kv)), blocks_(blocks) {}

core::Result<uint64_t> Pruner::prune_blocks_before(core::BlockHeight tip, uint64_t retain) {
    if (tip <= retain) return 0ULL;
    core::BlockHeight cutoff = tip - retain;
    uint64_t removed = 0;
    for (core::BlockHeight h = 0; h < cutoff; ++h) {
        auto b = blocks_.get_by_height(h);
        if (!b || !b.value()) continue;
        auto hash = b.value()->hash();
        core::Bytes key_h{'b', '/', 'h', '/'};
        key_h.insert(key_h.end(), hash.begin(), hash.end());
        core::Bytes key_n{'b', '/', 'n', '/'};
        for (int i = 7; i >= 0; --i)
            key_n.push_back(static_cast<uint8_t>((h >> (8 * i)) & 0xff));
        kv_->del(key_h);
        kv_->del(key_n);
        ++removed;
    }
    return removed;
}

}  // namespace l1::storage
