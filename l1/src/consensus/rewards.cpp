// =============================================================================
// rewards.cpp — Distribución de recompensas
// =============================================================================
#include "rewards.hpp"

namespace l1::consensus {

Rewards::Rewards(const core::ChainParams& params) : params_(params) {}

std::vector<std::pair<core::Address, core::Amount>>
Rewards::distribute(const core::Address& proposer,
                    const std::vector<core::Address>& voters,
                    core::Amount tips) const {
    std::vector<std::pair<core::Address, core::Amount>> out;
    out.emplace_back(proposer, params_.block_reward + tips);
    // Bonus simbólico a votantes (1 unidad cada uno) si hay tips sobrantes — simplificado.
    (void)voters;
    return out;
}

}  // namespace l1::consensus
