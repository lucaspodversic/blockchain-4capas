// =============================================================================
// finality_gadget.hpp — Finalidad Casper FFG-like con votos BLS
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include "validator_set.hpp"
#include <map>
#include <set>

namespace l1::consensus {

class FinalityGadget {
public:
    explicit FinalityGadget(const ValidatorSet& vs, uint32_t threshold_bps = 6667);

    /// Registra voto; si el stake acumulado ≥ umbral, justifica/finaliza.
    bool on_vote(const core::Vote& vote, core::Amount stake);
    core::Hash256 justified() const { return justified_; }
    core::Hash256 finalized() const { return finalized_; }
    void set_justified(const core::Hash256& h) { justified_ = h; }
    void set_finalized(const core::Hash256& h) { finalized_ = h; }

private:
    const ValidatorSet& vs_;
    uint32_t threshold_bps_;
    core::Hash256 justified_{core::zero_hash()};
    core::Hash256 finalized_{core::zero_hash()};
    std::map<core::Hash256, core::Amount> vote_weight_;
    std::map<core::Hash256, std::set<core::Address>> voters_;
};

}  // namespace l1::consensus
