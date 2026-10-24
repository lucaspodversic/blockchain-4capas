// =============================================================================
// test_core.cpp — Tests de tx, bloques y serialización
// =============================================================================
#include <gtest/gtest.h>
#include "../src/core/serialize.hpp"
#include "../src/core/transaction.hpp"
#include "../src/core/block.hpp"
#include "../src/core/genesis.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"

TEST(Core, EncodeDecodeU64) {
    auto b = l1::core::encode_u64(0x0102030405060708ULL);
    auto r = l1::core::decode_u64(b);
    ASSERT_TRUE(r);
    EXPECT_EQ(r.value(), 0x0102030405060708ULL);
}

TEST(Core, TransactionRoundtrip) {
    auto kp = l1::crypto::ed25519_generate();
    l1::core::Transaction tx;
    tx.type = l1::core::TxType::Transfer;
    tx.from = l1::crypto::address_from_ed25519(kp.public_key);
    tx.to = l1::core::zero_address();
    tx.to[19] = 0x42;
    tx.nonce = 7;
    tx.value = 1000;
    tx.gas_limit = 21000;
    tx.max_fee_per_gas = 100;
    tx.chain_id = 1337;
    auto sh = tx.signing_hash();
    tx.signature = l1::crypto::ed25519_sign(kp.private_key, sh);
    tx.has_signature = true;
    ASSERT_TRUE(tx.validate_structure());
    auto enc = tx.encode();
    auto dec = l1::core::Transaction::decode(enc);
    ASSERT_TRUE(dec);
    EXPECT_EQ(dec.value().nonce, 7u);
    EXPECT_EQ(dec.value().hash(), tx.hash());
}

TEST(Core, BlockTxRoot) {
    l1::core::Block b;
    b.header.height = 1;
    b.header.gas_limit = 1000000;
    b.compute_tx_root();
    EXPECT_EQ(b.header.tx_root, l1::core::zero_hash());
    EXPECT_TRUE(b.validate_integrity());
}

TEST(Core, GenesisJson) {
    l1::core::GenesisConfig g;
    g.params = l1::core::ChainParams::devnet();
    auto kp = l1::crypto::ed25519_generate();
    auto addr = l1::crypto::address_from_ed25519(kp.public_key);
    g.alloc[addr] = 42;
    g.validators.push_back({addr, kp.public_key, 100});
    auto js = g.to_json();
    auto back = l1::core::GenesisConfig::from_json(js);
    ASSERT_TRUE(back);
    EXPECT_EQ(back.value().alloc.at(addr), 42u);
    EXPECT_EQ(back.value().validators.size(), 1u);
}
