// =============================================================================
// epoch_manager.hpp — Épocas y rotación de validadores
// =============================================================================
#pragma once
#include "../core/chain_params.hpp"
#include "validator_set.hpp"

namespace l1::consensus {

class EpochManager {
public:
    EpochManager(const core::ChainParams& params, ValidatorSet& vs);
    core::Epoch epoch_of(core::Slot slot) const;
    core::Slot epoch_start(core::Epoch e) const;
    /// Llamar al cruzar de época: aplica activaciones pendientes (aquí no-op extendible).
    void on_epoch_transition(core::Epoch new_epoch);
    const core::Bytes& epoch_seed() const { return seed_; }
    void set_epoch_seed(core::Bytes seed) { seed_ = std::move(seed); }

private:
    core::ChainParams params_;
    ValidatorSet& vs_;
    core::Bytes seed_{};
    core::Epoch current_{0};
};

}  // namespace l1::consensus
