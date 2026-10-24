// =============================================================================
// finality_gadget.cpp — Finalidad por votos (2/3 stake)
// =============================================================================
#include "finality_gadget.hpp"

namespace l1::consensus {

FinalityGadget::FinalityGadget(const ValidatorSet& vs, uint32_t threshold_bps)
    : vs_(vs), threshold_bps_(threshold_bps) {}

bool FinalityGadget::on_vote(const core::Vote& vote, core::Amount stake) {
    auto& set = voters_[vote.block_hash];
    if (set.count(vote.validator)) return false;
    set.insert(vote.validator);
    vote_weight_[vote.block_hash] += stake;
    core::Amount need = vs_.total_stake() * threshold_bps_ / 10000;
    if (need == 0) need = 1;
    if (vote_weight_[vote.block_hash] >= need) {
        // FFG simplificado: justificar este; si ya había justificado el padre, finalizar padre.
        if (justified_ != core::zero_hash() && justified_ != vote.block_hash) {
            finalized_ = justified_;
        }
        justified_ = vote.block_hash;
        return true;
    }
    return false;
}

}  // namespace l1::consensus
