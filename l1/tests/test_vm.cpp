// =============================================================================
// test_vm.cpp — Tests de VM y gas
// =============================================================================
#include <gtest/gtest.h>
#include "../src/vm/wasm_vm.hpp"
#include "../src/vm/gas_meter.hpp"
#include "../src/vm/precompiles.hpp"
#include "../src/vm/native_contracts.hpp"
#include "../src/state/state_db.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"
#include <cstring>

static l1::core::Bytes make_add_program(uint64_t a, uint64_t b) {
    // L1BC: PUSH a, PUSH b, ADD, RETURN
    l1::core::Bytes code{'L','1','B','C', 1};
    code.push_back(0x01); // PUSH8
    for (int i = 0; i < 8; ++i) code.push_back(static_cast<uint8_t>((a >> (8*i)) & 0xff));
    code.push_back(0x01);
    for (int i = 0; i < 8; ++i) code.push_back(static_cast<uint8_t>((b >> (8*i)) & 0xff));
    code.push_back(0x03); // ADD
    code.push_back(0x40); // RETURN
    return code;
}

TEST(Vm, GasMeterOutOfGas) {
    l1::vm::GasMeter g(10);
    EXPECT_TRUE(g.charge(5));
    EXPECT_FALSE(g.charge(6));
}

TEST(Vm, DeployAndCallAdd) {
    auto vm = l1::vm::create_default_vm();
    l1::state::StateDB db;
    auto kp = l1::crypto::ed25519_generate();
    auto caller = l1::crypto::address_from_ed25519(kp.public_key);
    db.apply_alloc({{caller, 1'000'000ULL}});
    l1::core::Address caddr{};
    caddr[19] = 0xAA;
    auto code = make_add_program(2, 3);
    auto dep = vm->deploy(db, caddr, code, caller, 0, 100000);
    ASSERT_TRUE(dep);
    auto call = vm->call(db, caddr, {}, caller, 0, 100000);
    ASSERT_TRUE(call);
    EXPECT_TRUE(call.value().success);
    ASSERT_EQ(call.value().output.size(), 8u);
    uint64_t out = 0;
    std::memcpy(&out, call.value().output.data(), 8);
    EXPECT_EQ(out, 5u);
}

TEST(Vm, PrecompileKeccak) {
    auto addr = l1::vm::precompile_address(l1::vm::Precompile::Keccak256);
    auto r = l1::vm::run_precompile(addr, l1::core::Bytes{}, 1000);
    ASSERT_TRUE(r);
    EXPECT_EQ(r.value().output.size(), 32u);
}

TEST(Vm, NativeStake) {
    l1::state::StateDB db;
    auto kp = l1::crypto::ed25519_generate();
    auto caller = l1::crypto::address_from_ed25519(kp.public_key);
    db.apply_alloc({{caller, 5000}});
    l1::core::Bytes input(4);
    uint32_t sel = 0x01;
    std::memcpy(input.data(), &sel, 4);
    auto r = l1::vm::call_native(db, l1::vm::native_staking(), input, caller, 1000, 100000);
    ASSERT_TRUE(r);
    EXPECT_TRUE(r.value().success);
}
