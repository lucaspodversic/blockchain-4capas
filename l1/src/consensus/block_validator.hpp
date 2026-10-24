// =============================================================================
// block_validator.hpp — Valida bloques recibidos (firmas, roots, reglas)
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include "../core/chain_params.hpp"
#include "validator_set.hpp"
#include "proposer_selection.hpp"

namespace l1::consensus {

class BlockValidator {
public:
    BlockValidator(const core::ChainParams& params, const ValidatorSet& vs,
                   const ProposerSelection& ps);
    core::Result<void> validate(const core::Block& block, const core::BlockHeader& parent,
                                const core::Bytes& epoch_seed) const;
private:
    const core::ChainParams& params_;
    const ValidatorSet& vs_;
    const ProposerSelection& ps_;
};

}  // namespace l1::consensus
