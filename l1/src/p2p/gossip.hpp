// =============================================================================
// gossip.hpp — Propagación eficiente de txs y bloques (seen-set + broadcast)
// =============================================================================
#pragma once
#include "transport_tcp.hpp"
#include "messages.hpp"
#include <set>
#include <mutex>

namespace l1::p2p {

class Gossip {
public:
    explicit Gossip(TcpTransport& transport);
    void publish_tx(const core::Transaction& tx);
    void publish_block(const core::Block& block);
    void publish_vote(const core::Vote& vote);
    /// true si el hash no se había visto (primera vez).
    bool mark_seen(const core::Hash256& h);

private:
    TcpTransport& tr_;
    std::mutex mu_;
    std::set<core::Hash256> seen_;
};

}  // namespace l1::p2p
