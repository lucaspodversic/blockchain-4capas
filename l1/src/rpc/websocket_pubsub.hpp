// =============================================================================
// websocket_pubsub.hpp — Pub/sub de bloques
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include <functional>
#include <mutex>
#include <map>

namespace l1::rpc {

using BlockSub = std::function<void(const core::Block&)>;

class WsPubSub {
public:
    uint64_t subscribe_blocks(BlockSub cb);
    void unsubscribe(uint64_t id);
    void publish_block(const core::Block& b);
private:
    std::mutex mu_;
    uint64_t next_id_{0};
    std::map<uint64_t, BlockSub> subs_;
};

}  // namespace l1::rpc
