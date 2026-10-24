// =============================================================================
// node.hpp — Orquestación de todos los módulos del nodo L1
// =============================================================================
#pragma once
#include "config.hpp"
#include "../consensus/pos_engine.hpp"
#include "../mempool/mempool.hpp"
#include "../mempool/fee_market.hpp"
#include "../state/state_db.hpp"
#include "../storage/kv_store.hpp"
#include "../storage/block_store.hpp"
#include "../p2p/transport_tcp.hpp"
#include "../p2p/gossip.hpp"
#include "../p2p/peer_manager.hpp"
#include "../rpc/json_rpc_server.hpp"
#include "../rpc/websocket_pubsub.hpp"
#include "../vm/wasm_vm.hpp"
#include "l2_verifier.hpp"
#include <memory>
#include <thread>
#include <atomic>

namespace l1::node {

class Node {
public:
    explicit Node(NodeConfig cfg);
    ~Node();

    core::Result<void> start();
    void stop();
    void run_forever();  ///< bloquea hasta señal

    consensus::PosEngine* engine() { return engine_.get(); }
    state::StateDB& state() { return db_; }
    mempool::Mempool& mempool() { return *pool_; }

private:
    void slot_loop();

    NodeConfig cfg_;
    std::shared_ptr<storage::KvStore> kv_;
    std::unique_ptr<storage::BlockStore> blocks_;
    state::StateDB db_;
    std::unique_ptr<mempool::Mempool> pool_;
    std::unique_ptr<mempool::FeeMarket> fees_;
    std::unique_ptr<vm::VmInterface> vm_;
    consensus::ValidatorSet validators_;
    std::unique_ptr<consensus::PosEngine> engine_;
    p2p::TcpTransport transport_;
    std::unique_ptr<p2p::Gossip> gossip_;
    p2p::PeerManager peers_;
    std::unique_ptr<rpc::JsonRpcServer> rpc_;
    rpc::WsPubSub pubsub_;
    L2Verifier l2_;
    std::atomic<bool> running_{false};
    std::thread slot_thread_;
    core::Slot slot_{0};
    core::PrivateKey sk_{};
    core::PublicKey pk_{};
    core::Address addr_{};
};

}  // namespace l1::node
