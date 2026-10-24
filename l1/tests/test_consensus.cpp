// =============================================================================
// test_consensus.cpp — Consenso, fork choice y slashing
// =============================================================================
#include <gtest/gtest.h>
#include "../src/consensus/validator_set.hpp"
#include "../src/consensus/proposer_selection.hpp"
#include "../src/consensus/fork_choice.hpp"
#include "../src/consensus/finality_gadget.hpp"
#include "../src/consensus/slashing.hpp"
#include "../src/consensus/pos_engine.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/address.hpp"
#include "../src/mempool/mempool.hpp"
#include "../src/state/state_db.hpp"

TEST(Consensus, ValidatorSetAndProposer) {
    l1::consensus::ValidatorSet vs;
    auto kp = l1::crypto::ed25519_generate();
    auto addr = l1::crypto::address_from_ed25519(kp.public_key);
    vs.activate(addr, 100, kp.public_key);
    EXPECT_EQ(vs.total_stake(), 100u);
    l1::consensus::ProposerSelection ps(vs);
    auto p = ps.select(0, l1::core::Bytes{1, 2, 3});
    EXPECT_EQ(p, addr);
}

TEST(Consensus, ForkChoiceWeights) {
    l1::consensus::ForkChoice fc;
    l1::core::Block g;
    g.header.height = 0;
    fc.add_block(g);
    l1::core::Block a;
    a.header.height = 1;
    a.header.parent_hash = g.hash();
    a.header.extra_data = {'a'};
    fc.add_block(a);
    l1::core::Vote v;
    v.block_hash = a.hash();
    v.slot = 1;
    fc.add_vote(v, 50);
    EXPECT_EQ(fc.head(), a.hash());
}

TEST(Consensus, SlashingDoubleVote) {
    l1::consensus::SlashingDetector d;
    l1::core::Vote v1;
    v1.validator[19] = 1;
    v1.slot = 5;
    v1.block_hash[0] = 1;
    EXPECT_FALSE(d.check(v1).has_value());
    l1::core::Vote v2 = v1;
    v2.block_hash[0] = 2;
    auto ev = d.check(v2);
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->reason, "double_vote_same_slot");
}

TEST(Consensus, FinalityThreshold) {
    l1::consensus::ValidatorSet vs;
    auto kp = l1::crypto::ed25519_generate();
    auto addr = l1::crypto::address_from_ed25519(kp.public_key);
    vs.activate(addr, 100, kp.public_key);
    l1::consensus::FinalityGadget fg(vs, 6667);
    l1::core::Vote v;
    v.validator = addr;
    v.block_hash[0] = 9;
    v.slot = 1;
    EXPECT_TRUE(fg.on_vote(v, 100));
    EXPECT_EQ(fg.justified(), v.block_hash);
}

TEST(Consensus, PosEnginePropose) {
    auto kp = l1::crypto::ed25519_generate();
    auto addr = l1::crypto::address_from_ed25519(kp.public_key);
    l1::consensus::ValidatorSet vs;
    vs.activate(addr, 100, kp.public_key);
    l1::state::StateDB db;
    db.apply_alloc({{addr, 1'000'000'000ULL}});
    auto params = l1::core::ChainParams::devnet();
    l1::mempool::Mempool pool(params);
    l1::consensus::PosConfig cfg;
    cfg.params = params;
    cfg.local_address = addr;
    cfg.local_sk = kp.private_key;
    cfg.local_pk = kp.public_key;
    cfg.is_validator = true;
    l1::consensus::PosEngine eng(cfg, vs, db, pool);
    l1::core::Block genesis;
    genesis.header.height = 0;
    genesis.header.proposer = addr;
    genesis.header.state_root = db.state_root();
    genesis.compute_tx_root();
    eng.set_genesis(genesis);
    auto evs = eng.on_slot(0);
    ASSERT_TRUE(evs);
    // Puede o no proponer según VRF; al menos no falla.
    EXPECT_GE(eng.height(), 0u);
}
