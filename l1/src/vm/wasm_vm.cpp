// =============================================================================
// wasm_vm.cpp — VM de contratos: intérprete de bytecode propio (L1BC)
//
// Formato L1BC (sustituto ligero de WASM/wasmtime):
//   magic "L1BC" (4) | version u8=1 | code...
// Opcodes:
//   0x00 STOP
//   0x01 PUSH8 <u64 LE>     — apila inmediato
//   0x02 POP
//   0x03 ADD / 0x04 SUB / 0x05 MUL
//   0x10 SLOAD              — clave=top → valor u64
//   0x11 SSTORE             — value, key
//   0x20 KECCAK             — hash de 8 bytes del top (demo)
//   0x30 TRANSFER           — amount, addr20-as-u64-truncated (solo tests)
//   0x40 RETURN             — devuelve top como output u64 LE
//
// Swap-in wasmtime: crear WasmtimeVm : VmInterface con la misma semántica de gas.
// =============================================================================
#include "vm_interface.hpp"
#include "gas_meter.hpp"
#include "host_functions.hpp"
#include "contract_storage.hpp"
#include "../crypto/hash.hpp"
#include "../state/state_db.hpp"
#include <vector>
#include <memory>
#include <cstring>

namespace l1::vm {

namespace {

constexpr char MAGIC[4] = {'L', '1', 'B', 'C'};

bool is_l1bc(const core::Bytes& code) {
    return code.size() >= 5 && std::memcmp(code.data(), MAGIC, 4) == 0 && code[4] == 1;
}

core::Result<VmResult> exec_l1bc(state::StateDB& db, const core::Address& contract,
                                 const core::Bytes& code, const core::Bytes& input,
                                 const core::Address& caller, core::Amount value,
                                 core::Gas gas_limit) {
    VmResult vr;
    GasMeter gas(gas_limit);
    HostContext ctx{db, contract, caller, value, gas};
    std::vector<uint64_t> stack;
    size_t pc = 5;  // skip magic+ver

    auto push = [&](uint64_t v) -> core::Result<void> {
        auto r = gas.charge(GAS_BASE);
        if (!r) return r;
        stack.push_back(v);
        return core::Result<void>::Ok();
    };
    auto pop = [&]() -> core::Result<uint64_t> {
        auto r = gas.charge(GAS_BASE);
        if (!r) return r.error();
        if (stack.empty()) return {core::ErrorCode::VmTrap, "stack underflow"};
        uint64_t v = stack.back();
        stack.pop_back();
        return v;
    };

    // Si hay input, apila primeros 8 bytes como u64.
    if (input.size() >= 8) {
        uint64_t v = 0;
        std::memcpy(&v, input.data(), 8);
        stack.push_back(v);
    }

    while (pc < code.size()) {
        uint8_t op = code[pc++];
        switch (op) {
            case 0x00:  // STOP
                vr.success = true;
                vr.gas_used = gas.used();
                return vr;
            case 0x01: {  // PUSH8
                if (pc + 8 > code.size()) return {core::ErrorCode::VmTrap, "push eof"};
                uint64_t imm = 0;
                std::memcpy(&imm, code.data() + pc, 8);
                pc += 8;
                auto r = push(imm);
                if (!r) return r.error();
                break;
            }
            case 0x02: {
                auto r = pop();
                if (!r) return r.error();
                break;
            }
            case 0x03: case 0x04: case 0x05: {
                auto b = pop(); if (!b) return b.error();
                auto a = pop(); if (!a) return a.error();
                uint64_t r = 0;
                if (op == 0x03) r = a.value() + b.value();
                else if (op == 0x04) r = a.value() - b.value();
                else r = a.value() * b.value();
                auto p = push(r);
                if (!p) return p.error();
                break;
            }
            case 0x10: {  // SLOAD
                auto k = pop(); if (!k) return k.error();
                auto key = ContractStorage::key_u64(k.value());
                auto val = host_storage_load(ctx, key);
                if (!val) return val.error();
                uint64_t v = 0;
                if (val.value().size() >= 8) std::memcpy(&v, val.value().data(), 8);
                auto p = push(v);
                if (!p) return p.error();
                break;
            }
            case 0x11: {  // SSTORE  (value, key) — key on top
                auto k = pop(); if (!k) return k.error();
                auto v = pop(); if (!v) return v.error();
                core::Bytes vb(8);
                std::memcpy(vb.data(), &v.value(), 8);
                auto r = host_storage_store(ctx, ContractStorage::key_u64(k.value()), vb);
                if (!r) return r.error();
                break;
            }
            case 0x20: {
                auto v = pop(); if (!v) return v.error();
                core::Bytes b(8);
                std::memcpy(b.data(), &v.value(), 8);
                auto h = host_keccak(ctx, b);
                if (!h) return h.error();
                uint64_t out = 0;
                std::memcpy(&out, h.value().data(), 8);
                auto p = push(out);
                if (!p) return p.error();
                break;
            }
            case 0x40: {  // RETURN
                auto v = pop(); if (!v) return v.error();
                vr.output.resize(8);
                std::memcpy(vr.output.data(), &v.value(), 8);
                vr.success = true;
                vr.gas_used = gas.used();
                return vr;
            }
            default:
                return {core::ErrorCode::VmTrap, "unknown opcode"};
        }
    }
    vr.success = true;
    vr.gas_used = gas.used();
    return vr;
}

}  // namespace

class BytecodeVm : public VmInterface {
public:
    core::Result<VmResult> deploy(state::StateDB& db, const core::Address& contract,
                                  const core::Bytes& code, const core::Address& caller,
                                  core::Amount value, core::Gas gas_limit) override {
        // Guardar código y acreditar value.
        core::Account acc = db.get_account(contract).value_or(core::Account{});
        acc.code = code;
        acc.code_hash = crypto::keccak256(code);
        auto from = db.get_account(caller).value_or(core::Account{});
        if (from.balance < value) return {core::ErrorCode::InsufficientBalance, "deploy"};
        from.balance -= value;
        acc.balance += value;
        db.set_account(caller, from);
        db.set_account(contract, acc);
        VmResult vr;
        vr.success = true;
        vr.gas_used = 20000;  // coste fijo de deploy
        if (vr.gas_used > gas_limit) return {core::ErrorCode::VmOutOfGas, "deploy gas"};
        return vr;
    }

    core::Result<VmResult> call(state::StateDB& db, const core::Address& contract,
                                const core::Bytes& input, const core::Address& caller,
                                core::Amount value, core::Gas gas_limit) override {
        auto acc = db.get_account(contract);
        if (!acc || acc->code.empty()) {
            // EOA / sin código: solo transfer.
            if (value > 0) {
                auto from = db.get_account(caller).value_or(core::Account{});
                if (from.balance < value) return {core::ErrorCode::InsufficientBalance, "call"};
                from.balance -= value;
                db.set_account(caller, from);
                auto to = acc.value_or(core::Account{});
                to.balance += value;
                db.set_account(contract, to);
            }
            VmResult vr;
            vr.success = true;
            vr.gas_used = 0;
            return vr;
        }
        if (value > 0) {
            auto from = db.get_account(caller).value_or(core::Account{});
            if (from.balance < value) return {core::ErrorCode::InsufficientBalance, "call value"};
            from.balance -= value;
            db.set_account(caller, from);
            auto to = *acc;
            to.balance += value;
            db.set_account(contract, to);
            acc = db.get_account(contract);
        }
        if (!is_l1bc(acc->code)) {
            return {core::ErrorCode::ContractError, "bytecode no L1BC (wasmtime no vinculado)"};
        }
        return exec_l1bc(db, contract, acc->code, input, caller, value, gas_limit);
    }

    core::Result<core::Gas> estimate_gas(state::StateDB& db, const core::Address& contract,
                                         const core::Bytes& input, const core::Address& caller,
                                         core::Amount value) override {
        // Estimación: ejecutar con gas alto y devolver used.
        auto r = call(db, contract, input, caller, value, 10'000'000);
        if (!r) return r.error();
        return r.value().gas_used;
    }
};

std::unique_ptr<VmInterface> create_default_vm() {
    return std::make_unique<BytecodeVm>();
}

}  // namespace l1::vm
