// =============================================================================
// fork_choice.cpp — LMD-GHOST simplificado
// =============================================================================
#include "fork_choice.hpp"

namespace l1::consensus {

void ForkChoice::add_block(const core::Block& block) {
    auto h = block.hash();
    if (nodes_.count(h)) return;
    Node n;
    n.block = block;
    n.parent = block.header.parent_hash;
    n.weight = 0;
    nodes_[h] = std::move(n);
    if (block.header.height == 0) {
        genesis_ = h;
        has_genesis_ = true;
    }
}

void ForkChoice::add_vote(const core::Vote& vote, core::Amount stake) {
    auto it = nodes_.find(vote.block_hash);
    if (it == nodes_.end()) return;
    // Propaga peso hacia la raíz.
    core::Hash256 cur = vote.block_hash;
    while (nodes_.count(cur)) {
        nodes_[cur].weight += stake;
        if (nodes_[cur].block.header.height == 0) break;
        cur = nodes_[cur].parent;
    }
}

core::Hash256 ForkChoice::head() const {
    if (!has_genesis_) return core::zero_hash();
    core::Hash256 cur = genesis_;
    while (true) {
        core::Hash256 best = cur;
        core::Amount best_w = 0;
        bool found = false;
        for (const auto& [h, n] : nodes_) {
            if (n.parent == cur) {
                if (!found || n.weight > best_w ||
                    (n.weight == best_w && h > best)) {
                    best = h;
                    best_w = n.weight;
                    found = true;
                }
            }
        }
        if (!found) return cur;
        cur = best;
    }
}

std::optional<core::Block> ForkChoice::get(const core::Hash256& h) const {
    auto it = nodes_.find(h);
    if (it == nodes_.end()) return std::nullopt;
    return it->second.block;
}

core::BlockHeight ForkChoice::height_of(const core::Hash256& h) const {
    auto it = nodes_.find(h);
    if (it == nodes_.end()) return 0;
    return it->second.block.header.height;
}

}  // namespace l1::consensus
