// =============================================================================
// host_functions.cpp — Funciones host (storage, hash, transfer)
// =============================================================================
#include "host_functions.hpp"
#include "contract_storage.hpp"

namespace l1::vm {

core::Result<core::Bytes> host_storage_load(HostContext& ctx, const core::Bytes& key) {
    auto r = ctx.gas.charge(GAS_STORAGE_LOAD);
    if (!r) return r.error();
    ContractStorage cs(ctx.db, ctx.contract);
    return cs.load(key);
}

core::Result<void> host_storage_store(HostContext& ctx, const core::Bytes& key,
                                      const core::Bytes& value) {
    auto r = ctx.gas.charge(GAS_STORAGE_STORE);
    if (!r) return r;
    ContractStorage cs(ctx.db, ctx.contract);
    cs.store(key, value);
    return core::Result<void>::Ok();
}

core::Result<core::Hash256> host_keccak(HostContext& ctx, const core::Bytes& data) {
    auto r = ctx.gas.charge(GAS_HASH);
    if (!r) return r.error();
    return crypto::keccak256(data);
}

core::Result<void> host_transfer(HostContext& ctx, const core::Address& to, core::Amount amount) {
    auto r = ctx.gas.charge(GAS_TRANSFER);
    if (!r) return r;
    auto from = ctx.db.get_account(ctx.contract).value_or(core::Account{});
    if (from.balance < amount) return {core::ErrorCode::InsufficientBalance, "host transfer"};
    from.balance -= amount;
    ctx.db.set_account(ctx.contract, from);
    auto dest = ctx.db.get_account(to).value_or(core::Account{});
    dest.balance += amount;
    ctx.db.set_account(to, dest);
    return core::Result<void>::Ok();
}

}  // namespace l1::vm
