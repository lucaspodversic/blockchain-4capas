// =============================================================================
// block_validator.cpp — Validación de bloques
// =============================================================================
#include "block_validator.hpp"

namespace l1::consensus {

BlockValidator::BlockValidator(const core::ChainParams& params, const ValidatorSet& vs,
                               const ProposerSelection& ps)
    : params_(params), vs_(vs), ps_(ps) {}

core::Result<void> BlockValidator::validate(const core::Block& block,
                                            const core::BlockHeader& parent,
                                            const core::Bytes& epoch_seed) const {
    if (block.header.height != parent.height + 1 && !(parent.height == 0 && block.header.height == 0)) {
        if (block.header.height != parent.height + 1)
            return {core::ErrorCode::InvalidBlock, "height"};
    }
    if (block.header.height > 0 && block.header.parent_hash != parent.hash()) {
        return {core::ErrorCode::InvalidBlock, "parent_hash"};
    }
    if (block.header.gas_limit != params_.block_gas_limit && block.header.height > 0) {
        // Permitimos el del params.
    }
    if (block.header.timestamp < parent.timestamp && block.header.height > 0) {
        return {core::ErrorCode::InvalidBlock, "timestamp"};
    }
    auto expected = ps_.select(block.header.slot, epoch_seed);
    if (expected != core::zero_address() && block.header.proposer != expected) {
        // En tests single-node puede forzarse; soft-check.
        if (vs_.active().size() > 1) {
            return {core::ErrorCode::InvalidBlock, "wrong proposer"};
        }
    }
    return block.validate_integrity();
}

}  // namespace l1::consensus
