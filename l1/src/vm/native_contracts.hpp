// =============================================================================
// native_contracts.hpp — Contratos de sistema
// =============================================================================
#pragma once
#include "vm_interface.hpp"
#include "../state/state_db.hpp"

namespace l1::vm {
core::Address native_staking();
core::Address native_treasury();
core::Address native_l2_verifier();
core::Result<VmResult> call_native(state::StateDB& db, const core::Address& contract,
                                   const core::Bytes& input, const core::Address& caller,
                                   core::Amount value, core::Gas gas_limit);
}  // namespace l1::vm
