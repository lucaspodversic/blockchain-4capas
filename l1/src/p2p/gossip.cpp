// =============================================================================
// gossip.cpp — Gossip de txs/bloques/votos
// =============================================================================
#include "gossip.hpp"
#include "../crypto/hash.hpp"

namespace l1::p2p {

Gossip::Gossip(TcpTransport& transport) : tr_(transport) {}

bool Gossip::mark_seen(const core::Hash256& h) {
    std::lock_guard lock(mu_);
    if (seen_.count(h)) return false;
    seen_.insert(h);
    if (seen_.size() > 100000) seen_.clear();
    return true;
}

void Gossip::publish_tx(const core::Transaction& tx) {
    auto h = tx.hash();
    if (!mark_seen(h)) return;
    Message m{MsgType::Tx, tx};
    tr_.broadcast(m);
}

void Gossip::publish_block(const core::Block& block) {
    auto h = block.hash();
    if (!mark_seen(h)) return;
    Message m{MsgType::Block, block};
    tr_.broadcast(m);
}

void Gossip::publish_vote(const core::Vote& vote) {
    core::Bytes buf(vote.block_hash.begin(), vote.block_hash.end());
    buf.insert(buf.end(), vote.validator.begin(), vote.validator.end());
    auto h = crypto::keccak256(buf);
    if (!mark_seen(h)) return;
    Message m{MsgType::Vote, vote};
    tr_.broadcast(m);
}

}  // namespace l1::p2p
