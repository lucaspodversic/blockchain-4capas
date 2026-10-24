// =============================================================================
// state_transition.hpp — API de aplicación de una tx al estado
// =============================================================================
#pragma once
#include "state_db.hpp"
#include "../core/transaction.hpp"
#include "../core/chain_params.hpp"
#include "../vm/vm_interface.hpp"

namespace l1::state {

struct Receipt {
    bool success{false};
    core::Gas gas_used{0};
    core::Bytes output;
    core::Address contract_address{core::zero_address()};
    std::string error;
};

class StateTransition {
public:
    StateTransition(StateDB& db, const core::ChainParams& params, vm::VmInterface* vm = nullptr);
    core::Result<Receipt> apply(const core::Transaction& tx, core::Amount base_fee);
private:
    StateDB& db_;
    const core::ChainParams& params_;
    vm::VmInterface* vm_;
};

}  // namespace l1::state
