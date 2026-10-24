// =============================================================================
// block_sync.cpp — Sync de rangos de bloques
// =============================================================================
#include "block_sync.hpp"

namespace l1::p2p {

BlockSync::BlockSync(TcpTransport& tr, storage::BlockStore& store)
    : tr_(tr), store_(store) {}

void BlockSync::request_range(int peer_fd, core::BlockHeight from, core::BlockHeight to) {
    Message m;
    m.type = MsgType::GetBlocks;
    m.payload = GetBlocksMsg{from, to};
    tr_.send(peer_fd, m);
}

void BlockSync::on_get_blocks(int peer_fd, const GetBlocksMsg& req) {
    BlocksMsg out;
    for (core::BlockHeight h = req.from; h <= req.to; ++h) {
        auto b = store_.get_by_height(h);
        if (b && b.value()) out.blocks.push_back(*b.value());
    }
    Message m{MsgType::Blocks, out};
    tr_.send(peer_fd, m);
}

void BlockSync::on_blocks(const BlocksMsg& msg,
                          const std::function<void(const core::Block&)>& apply) {
    for (const auto& b : msg.blocks) apply(b);
}

}  // namespace l1::p2p
