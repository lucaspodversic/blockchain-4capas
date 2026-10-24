// =============================================================================
// proposer_selection.hpp — Selección de proposer
// =============================================================================
#pragma once
#include "validator_set.hpp"
#include "../crypto/hash.hpp"
#include "../crypto/vrf.hpp"

namespace l1::consensus {

class ProposerSelection {
public:
    explicit ProposerSelection(const ValidatorSet& vs);
    core::Address select(core::Slot slot, const core::Bytes& epoch_seed) const;
private:
    const ValidatorSet& vs_;
};

}  // namespace l1::consensus
