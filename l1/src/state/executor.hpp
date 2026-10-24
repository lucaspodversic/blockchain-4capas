// =============================================================================
// executor.hpp — Ejecutor de bloques
// =============================================================================
#pragma once
#include "state_db.hpp"
#include "state_transition.hpp"
#include "../core/block.hpp"
#include "../core/chain_params.hpp"
#include "../vm/vm_interface.hpp"
#include <vector>

namespace l1::state {

struct BlockExecutionResult {
    core::Hash256 state_root{};
    core::Gas gas_used{0};
    std::vector<Receipt> receipts;
};

class BlockExecutor {
public:
    BlockExecutor(StateDB& db, const core::ChainParams& params, vm::VmInterface* vm = nullptr);
    core::Result<BlockExecutionResult> execute(const core::Block& block);
private:
    StateDB& db_;
    const core::ChainParams& params_;
    vm::VmInterface* vm_;
};

}  // namespace l1::state
