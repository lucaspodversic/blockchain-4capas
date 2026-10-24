// =============================================================================
// epoch_manager.cpp — Manejo de épocas
// =============================================================================
#include "epoch_manager.hpp"
#include "../crypto/hash.hpp"

namespace l1::consensus {

EpochManager::EpochManager(const core::ChainParams& params, ValidatorSet& vs)
    : params_(params), vs_(vs) {
    seed_ = {0x01, 0x02, 0x03, 0x04};  // seed inicial determinista de testnet
}

core::Epoch EpochManager::epoch_of(core::Slot slot) const {
    return slot / params_.slots_per_epoch;
}

core::Slot EpochManager::epoch_start(core::Epoch e) const {
    return e * params_.slots_per_epoch;
}

void EpochManager::on_epoch_transition(core::Epoch new_epoch) {
    current_ = new_epoch;
    // Mezcla el seed con el número de época.
    core::Bytes buf = seed_;
    for (int i = 0; i < 8; ++i) buf.push_back(static_cast<uint8_t>((new_epoch >> (8 * i)) & 0xff));
    auto h = crypto::keccak256(buf);
    seed_.assign(h.begin(), h.end());
    (void)vs_;
}

}  // namespace l1::consensus
