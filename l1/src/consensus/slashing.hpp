// =============================================================================
// slashing.hpp — Detección de doble firma / surround votes
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace l1::consensus {

struct SlashEvidence {
    core::Address validator{};
    core::Vote vote_a{};
    core::Vote vote_b{};
    std::string reason;
};

class SlashingDetector {
public:
    /// Devuelve evidencia si el voto contradice uno previo del mismo validador.
    std::optional<SlashEvidence> check(const core::Vote& vote);

private:
    // validator → slot → vote
    std::map<core::Address, std::map<core::Slot, core::Vote>> history_;
};

}  // namespace l1::consensus
