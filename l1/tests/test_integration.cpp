// =============================================================================
// test_integration.cpp — Flujo punta a punta (estado + mempool + bloque + store)
// =============================================================================
#include <gtest/gtest.h>
#include "../src/state/state_db.hpp"
#include "../src/state/executor.hpp"
#include "../src/state/snapshot.hpp"
#include "../src/mempool/mempool.hpp"
#include "../src/storage/kv_store.hpp"
#include "../src/storage/block_store.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"

TEST(Integration, TransferInBlock) {
    auto alice_kp = l1::crypto::ed25519_generate();
    auto bob_kp = l1::crypto::ed25519_generate();
    auto alice = l1::crypto::address_from_ed25519(alice_kp.public_key);
    auto bob = l1::crypto::address_from_ed25519(bob_kp.public_key);

    l1::state::StateDB db;
    db.apply_alloc({{alice, 10'000'000ULL}});

    auto params = l1::core::ChainParams::devnet();
    l1::core::Transaction tx;
    tx.type = l1::core::TxType::Transfer;
    tx.from = alice;
    tx.to = bob;
    tx.nonce = 0;
    tx.value = 1000;
    tx.gas_limit = 21000;
    tx.max_fee_per_gas = params.initial_base_fee;
    tx.chain_id = params.chain_id;
    tx.signature = l1::crypto::ed25519_sign(alice_kp.private_key, tx.signing_hash());
    tx.has_signature = true;

    l1::mempool::Mempool pool(params);
    ASSERT_TRUE(pool.add(tx, params.initial_base_fee));

    l1::core::Block b;
    b.header.height = 1;
    b.header.gas_limit = params.block_gas_limit;
    b.header.base_fee = params.initial_base_fee;
    b.header.proposer = alice;
    b.transactions = pool.select(10, params.block_gas_limit);
    b.compute_tx_root();

    l1::state::BlockExecutor exec(db, params, nullptr);
    auto r = exec.execute(b);
    ASSERT_TRUE(r) << r.error().to_string();
    auto bob_acc = db.get_account(bob);
    ASSERT_TRUE(bob_acc);
    EXPECT_EQ(bob_acc->balance, 1000u);

    auto kv = std::shared_ptr<l1::storage::KvStore>(l1::storage::create_memory_store());
    l1::storage::BlockStore store(kv);
    ASSERT_TRUE(store.put_block(b));
    auto got = store.get_by_height(1);
    ASSERT_TRUE(got && got.value());
    EXPECT_EQ(got.value()->hash(), b.hash());
}

TEST(Integration, SnapshotRoundtrip) {
    l1::state::StateDB db;
    l1::core::Address a{};
    a[19] = 1;
    db.apply_alloc({{a, 42}});
    auto snap = l1::state::export_snapshot(db);
    auto back = l1::state::import_snapshot(snap);
    ASSERT_TRUE(back);
    EXPECT_EQ(back.value().get_account(a)->balance, 42u);
}
