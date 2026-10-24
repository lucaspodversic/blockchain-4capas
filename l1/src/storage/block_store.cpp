// =============================================================================
// block_store.cpp — Persistencia de bloques por hash y por altura
// =============================================================================
#include "block_store.hpp"
#include <cstring>

namespace l1::storage {

BlockStore::BlockStore(std::shared_ptr<KvStore> kv) : kv_(std::move(kv)) {}

core::Bytes BlockStore::prefix_hash(const core::Hash256& h) {
    core::Bytes k{'b', '/', 'h', '/'};
    k.insert(k.end(), h.begin(), h.end());
    return k;
}

core::Bytes BlockStore::prefix_height(core::BlockHeight height) {
    core::Bytes k{'b', '/', 'n', '/'};
    for (int i = 7; i >= 0; --i)
        k.push_back(static_cast<uint8_t>((height >> (8 * i)) & 0xff));
    return k;
}

core::Result<void> BlockStore::put_block(const core::Block& block) {
    auto hash = block.hash();
    auto r = kv_->put(prefix_hash(hash), block.encode());
    if (!r) return r;
    return kv_->put(prefix_height(block.header.height),
                    core::Bytes(hash.begin(), hash.end()));
}

core::Result<std::optional<core::Block>> BlockStore::get_by_hash(const core::Hash256& h) const {
    auto raw = kv_->get(prefix_hash(h));
    if (!raw) return raw.error();
    if (!raw.value()) return std::optional<core::Block>{};
    auto b = core::Block::decode(*raw.value());
    if (!b) return b.error();
    return std::optional<core::Block>{b.value()};
}

core::Result<std::optional<core::Block>> BlockStore::get_by_height(core::BlockHeight h) const {
    auto hv = kv_->get(prefix_height(h));
    if (!hv) return hv.error();
    if (!hv.value() || hv.value()->size() != 32) return std::optional<core::Block>{};
    core::Hash256 hash{};
    std::memcpy(hash.data(), hv.value()->data(), 32);
    return get_by_hash(hash);
}

core::Result<void> BlockStore::set_canonical_tip(const core::Hash256& h) {
    core::Bytes key{'b', '/', 'c', 'a', 'n', 'o', 'n'};
    return kv_->put(key, core::Bytes(h.begin(), h.end()));
}

core::Result<std::optional<core::Hash256>> BlockStore::canonical_tip() const {
    core::Bytes key{'b', '/', 'c', 'a', 'n', 'o', 'n'};
    auto v = kv_->get(key);
    if (!v) return v.error();
    if (!v.value() || v.value()->size() != 32) return std::optional<core::Hash256>{};
    core::Hash256 h{};
    std::memcpy(h.data(), v.value()->data(), 32);
    return std::optional<core::Hash256>(h);
}

}  // namespace l1::storage
