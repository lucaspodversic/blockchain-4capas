// =============================================================================
// test_p2p.cpp — Tests de red y gossip
// =============================================================================
#include <gtest/gtest.h>
#include "../src/p2p/codec.hpp"
#include "../src/p2p/handshake.hpp"
#include "../src/p2p/peer_manager.hpp"
#include "../src/p2p/discovery_kademlia.hpp"
#include "../src/p2p/rate_limiter.hpp"
#include "../src/p2p/gossip.hpp"
#include "../src/p2p/transport_tcp.hpp"
#include "../src/crypto/ed25519.hpp"
#include <thread>
#include <chrono>

TEST(P2P, CodecPingRoundtrip) {
    l1::p2p::Message m{l1::p2p::MsgType::Ping, l1::p2p::PingMsg{42}};
    auto frame = l1::p2p::encode_message(m);
    auto back = l1::p2p::decode_message(frame);
    ASSERT_TRUE(back);
    EXPECT_EQ(back.value().type, l1::p2p::MsgType::Ping);
    EXPECT_EQ(std::get<l1::p2p::PingMsg>(back.value().payload).nonce, 42u);
}

TEST(P2P, HandshakeChainId) {
    auto params = l1::core::ChainParams::devnet();
    auto kp = l1::crypto::ed25519_generate();
    auto hello = l1::p2p::make_hello(params, kp.public_key, "127.0.0.1:30303", 0, l1::core::zero_hash());
    auto ok = l1::p2p::check_hello(hello, params);
    EXPECT_TRUE(ok.ok);
    hello.chain_id = 999;
    auto bad = l1::p2p::check_hello(hello, params);
    EXPECT_FALSE(bad.ok);
}

TEST(P2P, PeerManagerBan) {
    l1::p2p::PeerManager pm;
    auto kp = l1::crypto::ed25519_generate();
    pm.add(1, kp.public_key, "a");
    for (int i = 0; i < 20; ++i) pm.penalize(1, 10);
    EXPECT_TRUE(pm.is_banned(kp.public_key));
}

TEST(P2P, KademliaClosest) {
    auto self = l1::crypto::ed25519_generate().public_key;
    l1::p2p::KademliaTable t(self);
    for (int i = 0; i < 5; ++i) {
        auto kp = l1::crypto::ed25519_generate();
        t.upsert({kp.public_key, "127.0.0.1", static_cast<uint16_t>(3000 + i)});
    }
    EXPECT_EQ(t.size(), 5u);
    auto c = t.closest(self, 3);
    EXPECT_EQ(c.size(), 3u);
}

TEST(P2P, RateLimiter) {
    l1::p2p::RateLimiter rl(2, std::chrono::milliseconds(1000));
    EXPECT_TRUE(rl.allow());
    EXPECT_TRUE(rl.allow());
    EXPECT_FALSE(rl.allow());
}

TEST(P2P, TcpLoopbackPing) {
    l1::p2p::TcpTransport server;
    std::atomic<bool> got{false};
    server.set_handler([&](int fd, const l1::p2p::Message& msg) {
        if (msg.type == l1::p2p::MsgType::Ping) {
            got = true;
            l1::p2p::Message pong{l1::p2p::MsgType::Pong,
                l1::p2p::PongMsg{std::get<l1::p2p::PingMsg>(msg.payload).nonce}};
            server.send(fd, pong);
        }
    });
    ASSERT_TRUE(server.listen("127.0.0.1", 19090));
    l1::p2p::TcpTransport client;
    auto fd = client.connect("127.0.0.1", 19090);
    ASSERT_TRUE(fd);
    l1::p2p::Message ping{l1::p2p::MsgType::Ping, l1::p2p::PingMsg{7}};
    ASSERT_TRUE(client.send(fd.value(), ping));
    for (int i = 0; i < 50 && !got; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_TRUE(got);
    client.stop();
    server.stop();
}
