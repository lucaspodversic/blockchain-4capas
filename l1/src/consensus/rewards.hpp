// =============================================================================
// rewards.hpp — Cálculo y distribución de recompensas
// =============================================================================
#pragma once
#include "../core/chain_params.hpp"
#include "validator_set.hpp"
#include <vector>
#include <utility>

namespace l1::consensus {

class Rewards {
public:
    explicit Rewards(const core::ChainParams& params);
    /// Recompensa al proposer + tip compartido entre votantes.
    std::vector<std::pair<core::Address, core::Amount>>
    distribute(const core::Address& proposer,
               const std::vector<core::Address>& voters,
               core::Amount tips) const;
private:
    core::ChainParams params_;
};

}  // namespace l1::consensus
