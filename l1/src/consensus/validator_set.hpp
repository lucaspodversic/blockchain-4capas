// =============================================================================
// validator_set.hpp — API del set de validadores
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/genesis.hpp"
#include <vector>
#include <map>
#include <optional>

namespace l1::consensus {

struct Validator {
    core::Address address{};
    core::PublicKey pubkey{};
    core::Amount stake{0};
    bool active{true};
};

class ValidatorSet {
public:
    void load_genesis(const std::vector<core::GenesisValidator>& gens);
    const std::vector<Validator>& validators() const;
    core::Amount total_stake() const;
    std::optional<Validator> get(const core::Address& a) const;
    void activate(const core::Address& a, core::Amount stake, const core::PublicKey& pk);
    void deactivate(const core::Address& a);
    std::vector<Validator> active() const;
private:
    std::vector<Validator> validators_;
    std::map<core::Address, size_t> by_addr_;
    core::Amount total_stake_{0};
};

}  // namespace l1::consensus
