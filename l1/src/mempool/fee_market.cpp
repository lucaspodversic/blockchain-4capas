// =============================================================================
// fee_market.cpp — Mercado de tarifas (base fee dinámica estilo EIP-1559)
// =============================================================================
#include "fee_market.hpp"

namespace l1::mempool {

FeeMarket::FeeMarket(const core::ChainParams& params)
    : params_(params), base_fee_(params.initial_base_fee) {}

core::Amount FeeMarket::base_fee() const { return base_fee_; }

void FeeMarket::on_block(core::Gas gas_used, core::Gas gas_limit) {
    core::Gas target = gas_limit / params_.fee_elasticity;
    if (target == 0) target = 1;
    if (gas_used == target) return;
    int64_t diff = static_cast<int64_t>(gas_used) - static_cast<int64_t>(target);
    int64_t delta = static_cast<int64_t>(base_fee_) * diff / static_cast<int64_t>(target) / 8;
    if (delta == 0) delta = (diff > 0) ? 1 : -1;
    int64_t next = static_cast<int64_t>(base_fee_) + delta;
    if (next < 1) next = 1;
    base_fee_ = static_cast<core::Amount>(next);
}

}  // namespace l1::mempool
