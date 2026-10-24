// =============================================================================
// slashing.cpp — Detección y registro de evidencia de slashing
// =============================================================================
#include "slashing.hpp"

namespace l1::consensus {

std::optional<SlashEvidence> SlashingDetector::check(const core::Vote& vote) {
    auto& by_slot = history_[vote.validator];
    auto it = by_slot.find(vote.slot);
    if (it != by_slot.end()) {
        if (it->second.block_hash != vote.block_hash) {
            SlashEvidence e;
            e.validator = vote.validator;
            e.vote_a = it->second;
            e.vote_b = vote;
            e.reason = "double_vote_same_slot";
            return e;
        }
        return std::nullopt;
    }
    by_slot[vote.slot] = vote;
    return std::nullopt;
}

}  // namespace l1::consensus
