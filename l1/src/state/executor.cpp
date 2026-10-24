// =============================================================================
// executor.cpp — Ejecuta un bloque completo y produce el nuevo state root
// =============================================================================
#include "executor.hpp"

namespace l1::state {

BlockExecutor::BlockExecutor(StateDB& db, const core::ChainParams& params, vm::VmInterface* vm)
    : db_(db), params_(params), vm_(vm) {}

core::Result<BlockExecutionResult> BlockExecutor::execute(const core::Block& block) {
    BlockExecutionResult out;
    StateTransition st(db_, params_, vm_);
    core::Gas total = 0;
    for (const auto& tx : block.transactions) {
        auto r = st.apply(tx, block.header.base_fee);
        if (!r) return r.error();
        total += r.value().gas_used;
        if (total > block.header.gas_limit) {
            return {core::ErrorCode::GasLimitExceeded, "block gas"};
        }
        out.receipts.push_back(std::move(r.value()));
    }
    if (block.header.proposer != core::zero_address()) {
        auto acc = db_.get_account(block.header.proposer).value_or(core::Account{});
        acc.balance += params_.block_reward;
        db_.set_account(block.header.proposer, acc);
        db_.commit();
    }
    out.gas_used = total;
    out.state_root = db_.state_root();
    return out;
}

}  // namespace l1::state
