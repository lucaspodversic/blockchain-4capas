// =============================================================================
// websocket_pubsub.cpp — Suscripciones in-process (base para WS real)
// =============================================================================
#include "websocket_pubsub.hpp"
#include <vector>

namespace l1::rpc {

uint64_t WsPubSub::subscribe_blocks(BlockSub cb) {
    std::lock_guard lock(mu_);
    uint64_t id = ++next_id_;
    subs_[id] = std::move(cb);
    return id;
}

void WsPubSub::unsubscribe(uint64_t id) {
    std::lock_guard lock(mu_);
    subs_.erase(id);
}

void WsPubSub::publish_block(const core::Block& b) {
    std::vector<BlockSub> copy;
    { std::lock_guard lock(mu_); for (auto& [_, cb] : subs_) copy.push_back(cb); }
    for (auto& cb : copy) cb(b);
}

}  // namespace l1::rpc
