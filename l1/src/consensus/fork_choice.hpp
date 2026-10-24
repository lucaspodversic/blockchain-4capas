// =============================================================================
// fork_choice.hpp — Regla LMD-GHOST simplificada (peso por stake de votos)
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include <map>
#include <vector>
#include <optional>

namespace l1::consensus {

class ForkChoice {
public:
    void add_block(const core::Block& block);
    void add_vote(const core::Vote& vote, core::Amount stake);
    core::Hash256 head() const;
    std::optional<core::Block> get(const core::Hash256& h) const;
    core::BlockHeight height_of(const core::Hash256& h) const;

private:
    struct Node {
        core::Block block;
        core::Amount weight{0};
        core::Hash256 parent{};
    };
    std::map<core::Hash256, Node> nodes_;
    core::Hash256 genesis_{};
    bool has_genesis_{false};
};

}  // namespace l1::consensus
