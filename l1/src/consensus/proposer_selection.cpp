// =============================================================================
// proposer_selection.cpp — Elige proposer por slot (stake + VRF)
// =============================================================================
#include "proposer_selection.hpp"
#include "../core/serialize.hpp"

namespace l1::consensus {

ProposerSelection::ProposerSelection(const ValidatorSet& vs) : vs_(vs) {}

core::Address ProposerSelection::select(core::Slot slot, const core::Bytes& epoch_seed) const {
    auto active = vs_.active();
    if (active.empty()) return core::zero_address();
    core::Amount total = 0;
    for (const auto& v : active) total += v.stake;
    if (total == 0) return active[0].address;
    core::Bytes alpha = epoch_seed;
    core::Encoder e(alpha);
    e.u64(slot);
    auto h = crypto::keccak256(alpha);
    uint64_t r = crypto::vrf_to_uint64(h);
    core::Amount pick = static_cast<core::Amount>(r % total);
    core::Amount acc = 0;
    for (const auto& v : active) {
        acc += v.stake;
        if (pick < acc) return v.address;
    }
    return active.back().address;
}

}  // namespace l1::consensus
