// =============================================================================
// state_transition.cpp — Aplica una transacción al estado
// =============================================================================
#include "state_transition.hpp"
#include "../crypto/hash.hpp"
#include <cstring>

namespace l1::state {

StateTransition::StateTransition(StateDB& db, const core::ChainParams& params,
                                 vm::VmInterface* vm)
    : db_(db), params_(params), vm_(vm) {}

core::Result<Receipt> StateTransition::apply(const core::Transaction& tx,
                                             core::Amount base_fee) {
    Receipt rc;
    auto snap = db_.snapshot();

    auto sender = db_.get_account(tx.from);
    core::Account from_acc = sender.value_or(core::Account{});
    if (tx.nonce != from_acc.nonce) {
        db_.revert(snap);
        return {core::ErrorCode::InvalidNonce, "nonce"};
    }
    core::Gas gas = tx.gas_limit;
    if (gas < params_.tx_base_gas) {
        db_.revert(snap);
        return {core::ErrorCode::InsufficientGas, "gas < base"};
    }
    core::Amount effective_fee = tx.max_fee_per_gas;
    if (effective_fee < base_fee) {
        db_.revert(snap);
        return {core::ErrorCode::InsufficientGas, "fee < base"};
    }
    core::Amount max_cost = tx.value + effective_fee * gas;
    if (from_acc.balance < max_cost) {
        db_.revert(snap);
        return {core::ErrorCode::InsufficientBalance, "balance"};
    }

    from_acc.balance -= effective_fee * gas;
    from_acc.nonce += 1;
    db_.set_account(tx.from, from_acc);

    core::Gas used = params_.tx_base_gas;
    core::Bytes output;

    if (tx.type == core::TxType::Transfer ||
        (tx.type == core::TxType::Call && tx.data.empty() && !vm_)) {
        from_acc = db_.get_account(tx.from).value_or(core::Account{});
        if (from_acc.balance < tx.value) {
            db_.revert(snap);
            return {core::ErrorCode::InsufficientBalance, "value"};
        }
        from_acc.balance -= tx.value;
        db_.set_account(tx.from, from_acc);
        auto to_acc = db_.get_account(tx.to).value_or(core::Account{});
        to_acc.balance += tx.value;
        db_.set_account(tx.to, to_acc);
        rc.success = true;
    } else if (tx.type == core::TxType::Deploy) {
        core::Bytes buf(tx.from.begin(), tx.from.end());
        auto n = tx.nonce;
        for (int i = 0; i < 8; ++i) buf.push_back(static_cast<uint8_t>((n >> (8 * i)) & 0xff));
        auto h = crypto::keccak256(buf);
        core::Address caddr{};
        std::memcpy(caddr.data(), h.data() + 12, 20);
        if (vm_) {
            auto dep = vm_->deploy(db_, caddr, tx.data, tx.from, tx.value, gas - used);
            if (!dep) { db_.revert(snap); return dep.error(); }
            used += dep.value().gas_used;
            output = dep.value().output;
        } else {
            core::Account cacc;
            cacc.code = tx.data;
            cacc.code_hash = crypto::keccak256(tx.data);
            from_acc = db_.get_account(tx.from).value_or(core::Account{});
            if (from_acc.balance < tx.value) {
                db_.revert(snap);
                return {core::ErrorCode::InsufficientBalance, "deploy value"};
            }
            from_acc.balance -= tx.value;
            cacc.balance = tx.value;
            db_.set_account(tx.from, from_acc);
            db_.set_account(caddr, cacc);
        }
        rc.contract_address = caddr;
        rc.success = true;
    } else {  // Call
        if (vm_) {
            auto call = vm_->call(db_, tx.to, tx.data, tx.from, tx.value, gas - used);
            if (!call) { db_.revert(snap); return call.error(); }
            used += call.value().gas_used;
            output = call.value().output;
            rc.success = call.value().success;
        } else {
            from_acc = db_.get_account(tx.from).value_or(core::Account{});
            if (from_acc.balance < tx.value) {
                db_.revert(snap);
                return {core::ErrorCode::InsufficientBalance, "call value"};
            }
            from_acc.balance -= tx.value;
            db_.set_account(tx.from, from_acc);
            auto to_acc = db_.get_account(tx.to).value_or(core::Account{});
            to_acc.balance += tx.value;
            db_.set_account(tx.to, to_acc);
            rc.success = true;
        }
    }

    core::Gas refund = (gas > used) ? (gas - used) : 0;
    from_acc = db_.get_account(tx.from).value_or(core::Account{});
    from_acc.balance += effective_fee * refund;
    db_.set_account(tx.from, from_acc);

    rc.gas_used = used;
    rc.output = std::move(output);
    db_.commit();
    return rc;
}

}  // namespace l1::state
