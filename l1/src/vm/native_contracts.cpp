// =============================================================================
// native_contracts.cpp — Contratos de sistema: staking, tesorería, verificador L2
// Se invocan por direcciones fijas 0x1000 / 0x1001 / 0x1002.
// =============================================================================
#include "../state/state_db.hpp"
#include "../core/serialize.hpp"
#include "../core/error.hpp"
#include "vm_interface.hpp"
#include <cstring>

namespace l1::vm {

core::Address native_staking() {
    core::Address a{}; a[18] = 0x10; a[19] = 0x00; return a;
}
core::Address native_treasury() {
    core::Address a{}; a[18] = 0x10; a[19] = 0x01; return a;
}
core::Address native_l2_verifier() {
    core::Address a{}; a[18] = 0x10; a[19] = 0x02; return a;
}

/// Selector: primeros 4 bytes del input.
enum class NativeOp : uint32_t {
    Stake = 0x01,
    Unstake = 0x02,
    DepositTreasury = 0x10,
    VerifyL2Batch = 0x20,
};

core::Result<VmResult> call_native(state::StateDB& db, const core::Address& contract,
                                   const core::Bytes& input, const core::Address& caller,
                                   core::Amount value, core::Gas gas_limit) {
    VmResult vr;
    if (gas_limit < 5000) return {core::ErrorCode::VmOutOfGas, "native"};
    if (input.size() < 4) return {core::ErrorCode::InvalidArgument, "native input"};
    uint32_t sel = 0;
    std::memcpy(&sel, input.data(), 4);

    if (contract == native_staking() && sel == static_cast<uint32_t>(NativeOp::Stake)) {
        // Acredita stake en storage del contrato bajo clave = caller.
        auto acc = db.get_account(caller).value_or(core::Account{});
        if (acc.balance < value) return {core::ErrorCode::InsufficientBalance, "stake"};
        acc.balance -= value;
        db.set_account(caller, acc);
        auto st = db.get_account(contract).value_or(core::Account{});
        st.balance += value;
        db.set_account(contract, st);
        core::Bytes key(caller.begin(), caller.end());
        auto prev = db.get_storage(contract, key);
        uint64_t cur = 0;
        if (prev && prev.value().size() >= 8) std::memcpy(&cur, prev.value().data(), 8);
        cur += value;
        core::Bytes vb(8);
        std::memcpy(vb.data(), &cur, 8);
        db.set_storage(contract, key, vb);
        vr.success = true;
        vr.gas_used = 5000;
        return vr;
    }

    if (contract == native_l2_verifier() &&
        sel == static_cast<uint32_t>(NativeOp::VerifyL2Batch)) {
        // input: sel(4) || prev_root(32) || new_root(32) || proof_hash(32)
        if (input.size() < 4 + 96) return {core::ErrorCode::InvalidArgument, "batch"};
        // Placeholder: acepta cualquier prueba no nula (L1 l2_verifier.cpp hace más).
        bool nonzero = false;
        for (size_t i = 4 + 64; i < 4 + 96; ++i) if (input[i]) nonzero = true;
        vr.success = nonzero;
        vr.output.push_back(nonzero ? 1 : 0);
        vr.gas_used = 50000;
        return vr;
    }

    if (contract == native_treasury()) {
        auto st = db.get_account(contract).value_or(core::Account{});
        st.balance += value;
        db.set_account(contract, st);
        if (value > 0) {
            auto acc = db.get_account(caller).value_or(core::Account{});
            // value ya se descuenta en state_transition; aquí solo recibimos.
        }
        vr.success = true;
        vr.gas_used = 3000;
        return vr;
    }

    return {core::ErrorCode::ContractError, "unknown native op"};
}

}  // namespace l1::vm
