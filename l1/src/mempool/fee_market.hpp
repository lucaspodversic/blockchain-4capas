// =============================================================================
// fee_market.hpp — Base fee EIP-1559-like
// =============================================================================
#pragma once
#include "../core/chain_params.hpp"

namespace l1::mempool {

class FeeMarket {
public:
    explicit FeeMarket(const core::ChainParams& params);
    core::Amount base_fee() const;
    void on_block(core::Gas gas_used, core::Gas gas_limit);
private:
    core::ChainParams params_;
    core::Amount base_fee_;
};

}  // namespace l1::mempool
