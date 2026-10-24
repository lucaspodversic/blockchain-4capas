// =============================================================================
// block_sync.hpp — Sincronización de bloques para nodos nuevos
// =============================================================================
#pragma once
#include "transport_tcp.hpp"
#include "../storage/block_store.hpp"

namespace l1::p2p {

class BlockSync {
public:
    BlockSync(TcpTransport& tr, storage::BlockStore& store);
    void request_range(int peer_fd, core::BlockHeight from, core::BlockHeight to);
    void on_get_blocks(int peer_fd, const GetBlocksMsg& req);
    void on_blocks(const BlocksMsg& msg,
                   const std::function<void(const core::Block&)>& apply);

private:
    TcpTransport& tr_;
    storage::BlockStore& store_;
};

}  // namespace l1::p2p
