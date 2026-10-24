// =============================================================================
// test_crypto.cpp — Tests de hashes y firmas
// =============================================================================
#include <gtest/gtest.h>
#include "../src/crypto/hash.hpp"
#include "../src/crypto/ed25519.hpp"
#include "../src/crypto/secp256k1.hpp"
#include "../src/crypto/merkle_tree.hpp"
#include "../src/crypto/address.hpp"
#include "../src/crypto/keystore.hpp"
#include "../src/crypto/vrf.hpp"
#include "../src/crypto/bls.hpp"

TEST(Crypto, Sha256Empty) {
    auto h = l1::crypto::sha256(nullptr, 0);
    // e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    EXPECT_EQ(l1::core::to_hex(h, false),
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(Crypto, KeccakEmpty) {
    auto h = l1::crypto::keccak256(nullptr, 0);
    EXPECT_EQ(l1::core::to_hex(h, false),
              "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470");
}

TEST(Crypto, Blake3NotZero) {
    const char* msg = "hello";
    auto h = l1::crypto::blake3(reinterpret_cast<const uint8_t*>(msg), 5);
    EXPECT_NE(h, l1::core::zero_hash());
}

TEST(Crypto, Ed25519SignVerify) {
    auto kp = l1::crypto::ed25519_generate();
    l1::core::Bytes msg{'h', 'i'};
    auto sig = l1::crypto::ed25519_sign(kp.private_key, msg);
    EXPECT_TRUE(l1::crypto::ed25519_verify(kp.public_key, msg, sig));
    msg[0] = 'H';
    EXPECT_FALSE(l1::crypto::ed25519_verify(kp.public_key, msg, sig));
}

TEST(Crypto, Secp256k1SignVerify) {
    auto kp = l1::crypto::secp256k1_generate();
    auto dig = l1::crypto::keccak256(l1::core::Bytes{'a', 'b'});
    auto sig = l1::crypto::secp256k1_sign(kp.private_key, dig);
    ASSERT_TRUE(sig);
    EXPECT_TRUE(l1::crypto::secp256k1_verify(kp.public_key, dig, sig.value()));
    auto addr = l1::crypto::secp256k1_address(kp.public_key);
    EXPECT_NE(addr, l1::core::zero_address());
}

TEST(Crypto, MerkleRootAndProof) {
    std::vector<l1::core::Hash256> leaves;
    for (int i = 0; i < 4; ++i) {
        l1::core::Bytes b{static_cast<uint8_t>(i)};
        leaves.push_back(l1::crypto::keccak256(b));
    }
    auto root = l1::crypto::MerkleTree::root(leaves);
    auto proof = l1::crypto::MerkleTree::prove(leaves, 1);
    EXPECT_TRUE(l1::crypto::MerkleTree::verify(leaves[1], proof, root));
}

TEST(Crypto, AddressChecksum) {
    auto kp = l1::crypto::ed25519_generate();
    auto addr = l1::crypto::address_from_ed25519(kp.public_key);
    auto cs = l1::crypto::address_to_checksum(addr);
    EXPECT_EQ(cs.substr(0, 2), "0x");
    EXPECT_EQ(cs.size(), 42u);
}

TEST(Crypto, KeystoreRoundtrip) {
    auto kp = l1::crypto::ed25519_generate();
    auto enc = l1::crypto::keystore_encrypt(kp.private_key, "secret");
    ASSERT_TRUE(enc);
    auto dec = l1::crypto::keystore_decrypt(enc.value(), "secret");
    ASSERT_TRUE(dec);
    EXPECT_EQ(dec.value().private_key, kp.private_key);
    auto bad = l1::crypto::keystore_decrypt(enc.value(), "wrong");
    EXPECT_FALSE(bad);
}

TEST(Crypto, Vrf) {
    auto kp = l1::crypto::ed25519_generate();
    l1::core::Bytes alpha{'s', 'l', 'o', 't'};
    auto out = l1::crypto::vrf_prove(kp.private_key, alpha);
    EXPECT_TRUE(l1::crypto::vrf_verify(kp.public_key, alpha, out));
}

TEST(Crypto, BlsAggregate) {
    auto a = l1::crypto::bls_generate();
    auto b = l1::crypto::bls_generate();
    auto msg = l1::crypto::keccak256(l1::core::Bytes{1});
    auto sa = l1::crypto::bls_sign(a.secret_key, msg);
    auto sb = l1::crypto::bls_sign(b.secret_key, msg);
    auto agg = l1::crypto::bls_aggregate({sa, sb});
    EXPECT_TRUE(l1::crypto::bls_fast_aggregate_verify({a.public_key, b.public_key}, msg, agg));
}
