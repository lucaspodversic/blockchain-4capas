// =============================================================================
// pos_engine.hpp — Motor Proof-of-Stake: propone, vota y confirma bloques
// =============================================================================
#pragma once
#include "consensus_engine.hpp"
#include "validator_set.hpp"
#include "proposer_selection.hpp"
#include "block_validator.hpp"
#include "fork_choice.hpp"
#include "finality_gadget.hpp"
#include "slashing.hpp"
#include "epoch_manager.hpp"
#include "../core/chain_params.hpp"
#include "../mempool/mempool.hpp"
#include "../state/state_db.hpp"
#include "../state/executor.hpp"
#include <functional>
#include <memory>

namespace l1::consensus {

struct PosConfig {
    core::ChainParams params{core::ChainParams::devnet()};
    core::Address local_address{core::zero_address()};
    core::PrivateKey local_sk{};
    core::PublicKey local_pk{};
    bool is_validator{false};
};

class PosEngine : public ConsensusEngine {
public:
    PosEngine(PosConfig cfg, ValidatorSet vs, state::StateDB& db,
              mempool::Mempool& pool);

    core::Result<std::vector<ConsensusEvent>> on_slot(core::Slot slot) override;
    core::Result<std::vector<ConsensusEvent>> on_block(const core::Block& block) override;
    core::Result<std::vector<ConsensusEvent>> on_vote(const core::Vote& vote) override;

    core::Hash256 head() const override;
    core::Hash256 finalized() const override;
    core::BlockHeight height() const override;

    ForkChoice& fork_choice() { return fc_; }
    FinalityGadget& finality() { return finality_; }
    ValidatorSet& validators() { return vs_; }

    /// Inyecta el bloque génesis.
    void set_genesis(const core::Block& genesis);

private:
    core::Result<core::Block> propose(core::Slot slot);

    PosConfig cfg_;
    ValidatorSet vs_;
    ProposerSelection ps_;
    BlockValidator bval_;
    ForkChoice fc_;
    FinalityGadget finality_;
    SlashingDetector slash_;
    EpochManager epochs_;
    state::StateDB& db_;
    mempool::Mempool& pool_;
    core::BlockHeader last_header_{};
    bool has_parent_{false};
};

}  // namespace l1::consensus
