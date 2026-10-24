// =============================================================================
// validator_set.cpp — Set de validadores, stake y activación/salida
// =============================================================================
#include "validator_set.hpp"

namespace l1::consensus {

void ValidatorSet::load_genesis(const std::vector<core::GenesisValidator>& gens) {
    validators_.clear();
    by_addr_.clear();
    total_stake_ = 0;
    for (const auto& g : gens) {
        Validator v{g.address, g.pubkey, g.stake, true};
        validators_.push_back(v);
        by_addr_[g.address] = validators_.size() - 1;
        total_stake_ += g.stake;
    }
}

const std::vector<Validator>& ValidatorSet::validators() const { return validators_; }
core::Amount ValidatorSet::total_stake() const { return total_stake_; }

std::optional<Validator> ValidatorSet::get(const core::Address& a) const {
    auto it = by_addr_.find(a);
    if (it == by_addr_.end()) return std::nullopt;
    return validators_[it->second];
}

void ValidatorSet::activate(const core::Address& a, core::Amount stake, const core::PublicKey& pk) {
    auto it = by_addr_.find(a);
    if (it != by_addr_.end()) {
        auto& v = validators_[it->second];
        if (v.active) total_stake_ -= v.stake;
        v.stake = stake;
        v.pubkey = pk;
        v.active = true;
        total_stake_ += stake;
        return;
    }
    validators_.push_back(Validator{a, pk, stake, true});
    by_addr_[a] = validators_.size() - 1;
    total_stake_ += stake;
}

void ValidatorSet::deactivate(const core::Address& a) {
    auto it = by_addr_.find(a);
    if (it == by_addr_.end()) return;
    auto& v = validators_[it->second];
    if (v.active) { v.active = false; total_stake_ -= v.stake; }
}

std::vector<Validator> ValidatorSet::active() const {
    std::vector<Validator> out;
    for (const auto& v : validators_) if (v.active) out.push_back(v);
    return out;
}

}  // namespace l1::consensus
