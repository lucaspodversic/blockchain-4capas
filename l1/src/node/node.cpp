// =============================================================================
// node.cpp — Arranque, bucle principal y apagado del nodo
// =============================================================================
#include "node.hpp"
#include "logger.hpp"
#include "metrics.hpp"
#include "signal_handler.hpp"
#include "../core/genesis.hpp"
#include "../crypto/ed25519.hpp"
#include "../crypto/address.hpp"
#include "../crypto/keystore.hpp"
#include <fstream>
#include <chrono>
#include <thread>

namespace l1::node {

Node::Node(NodeConfig cfg) : cfg_(std::move(cfg)) {
    if (cfg_.use_rocksdb) kv_ = storage::create_rocksdb_store(cfg_.data_dir + "/db");
    else kv_ = storage::create_memory_store();
    blocks_ = std::make_unique<storage::BlockStore>(kv_);
    pool_ = std::make_unique<mempool::Mempool>(cfg_.params);
    fees_ = std::make_unique<mempool::FeeMarket>(cfg_.params);
    vm_ = vm::create_default_vm();
}

Node::~Node() { stop(); }

core::Result<void> Node::start() {
    // Claves
    if (!cfg_.keystore_path.empty()) {
        std::ifstream f(cfg_.keystore_path);
        if (!f) return {core::ErrorCode::ConfigError, "keystore missing"};
        std::string content((std::istreambuf_iterator<char>(f)), {});
        auto ks = crypto::keystore_decrypt(content, cfg_.keystore_password);
        if (!ks) return ks.error();
        sk_ = ks.value().private_key;
        pk_ = crypto::ed25519_derive_public(sk_);
        addr_ = ks.value().address;
    } else {
        auto kp = crypto::ed25519_generate();
        sk_ = kp.private_key;
        pk_ = kp.public_key;
        addr_ = crypto::address_from_ed25519(pk_);
        log_warn("sin keystore: clave efímera " + core::to_hex(addr_));
    }

    // Génesis
    core::GenesisConfig genesis;
    {
        std::ifstream f(cfg_.genesis_path);
        if (f) {
            std::string content((std::istreambuf_iterator<char>(f)), {});
            auto g = core::GenesisConfig::from_json(content);
            if (!g) return g.error();
            genesis = g.value();
            cfg_.params = genesis.params;
        } else {
            log_warn("genesis.json no encontrado; usando alloc vacío + este nodo como validador");
            genesis.params = cfg_.params;
            genesis.validators.push_back({addr_, pk_, cfg_.params.min_validator_stake});
            genesis.alloc[addr_] = 1'000'000'000'000ULL;
        }
    }
    db_.apply_alloc(genesis.alloc);
    auto gblock = genesis.build_block();
    gblock.header.state_root = db_.state_root();
    gblock.header.proposer = addr_;
    blocks_->put_block(gblock);
    blocks_->set_canonical_tip(gblock.hash());

    validators_.load_genesis(genesis.validators);
    if (validators_.active().empty()) {
        validators_.activate(addr_, cfg_.params.min_validator_stake, pk_);
    }

    consensus::PosConfig pcfg;
    pcfg.params = cfg_.params;
    pcfg.local_address = addr_;
    pcfg.local_sk = sk_;
    pcfg.local_pk = pk_;
    pcfg.is_validator = cfg_.validator || true;  // en devnet siempre proponemos si somos el único

    engine_ = std::make_unique<consensus::PosEngine>(pcfg, validators_, db_, *pool_);
    engine_->set_genesis(gblock);

    gossip_ = std::make_unique<p2p::Gossip>(transport_);
    transport_.set_handler([this](int fd, const p2p::Message& msg) {
        Metrics::instance().inc("p2p_messages_total");
        if (msg.type == p2p::MsgType::Tx) {
            auto& tx = std::get<core::Transaction>(msg.payload);
            if (gossip_->mark_seen(tx.hash())) {
                pool_->add(tx, fees_->base_fee());
                gossip_->publish_tx(tx);
            }
        } else if (msg.type == p2p::MsgType::Block) {
            auto& b = std::get<core::Block>(msg.payload);
            if (gossip_->mark_seen(b.hash())) {
                engine_->on_block(b);
                blocks_->put_block(b);
                gossip_->publish_block(b);
                pubsub_.publish_block(b);
            }
        } else if (msg.type == p2p::MsgType::Vote) {
            engine_->on_vote(std::get<core::Vote>(msg.payload));
        } else if (msg.type == p2p::MsgType::Ping) {
            p2p::Message pong{p2p::MsgType::Pong, p2p::PongMsg{std::get<p2p::PingMsg>(msg.payload).nonce}};
            transport_.send(fd, pong);
        }
        (void)fd;
    });

    auto lr = transport_.listen(cfg_.listen_p2p, cfg_.p2p_port);
    if (!lr) log_warn("P2P listen failed: " + lr.error().to_string());

    for (const auto& bn : cfg_.bootnodes) {
        auto colon = bn.find(':');
        if (colon == std::string::npos) continue;
        auto host = bn.substr(0, colon);
        auto port = static_cast<uint16_t>(std::stoi(bn.substr(colon + 1)));
        transport_.connect(host, port);
    }

    rpc::RpcContext rctx;
    rctx.db = &db_;
    rctx.blocks = blocks_.get();
    rctx.pool = pool_.get();
    rctx.fees = fees_.get();
    rctx.consensus = engine_.get();
    rctx.params = cfg_.params;
    rctx.submit_tx = [this](const core::Transaction& tx) -> core::Result<core::Hash256> {
        auto r = pool_->add(tx, fees_->base_fee());
        if (!r) return r.error();
        gossip_->publish_tx(tx);
        return tx.hash();
    };
    rpc::RpcAuth auth;
    if (!cfg_.api_token.empty()) auth.set_api_token(cfg_.api_token);
    rpc_ = std::make_unique<rpc::JsonRpcServer>(std::move(rctx), std::move(auth));
    auto rr = rpc_->start(cfg_.listen_rpc, cfg_.rpc_port);
    if (!rr) return rr;

    running_ = true;
    slot_thread_ = std::thread([this] { slot_loop(); });
    log_info("nodo L1 arrancado rpc=" + cfg_.listen_rpc + ":" + std::to_string(cfg_.rpc_port) +
             " p2p=:" + std::to_string(cfg_.p2p_port));
    return core::Result<void>::Ok();
}

void Node::slot_loop() {
    while (running_ && !g_shutdown.load()) {
        auto evs = engine_->on_slot(slot_);
        if (evs) {
            for (auto& e : evs.value()) {
                if (e.kind == consensus::ConsensusEvent::Kind::Proposed) {
                    blocks_->put_block(e.block);
                    blocks_->set_canonical_tip(e.block_hash);
                    fees_->on_block(e.block.header.gas_used, e.block.header.gas_limit);
                    gossip_->publish_block(e.block);
                    pubsub_.publish_block(e.block);
                    Metrics::instance().inc("blocks_proposed_total");
                    Metrics::instance().set_gauge("block_height", static_cast<double>(e.block.header.height));
                    log_info("bloque propuesto height=" + std::to_string(e.block.header.height) +
                             " hash=" + core::to_hex(e.block_hash));
                }
            }
        }
        ++slot_;
        std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.params.slot_ms));
    }
}

void Node::stop() {
    running_ = false;
    if (slot_thread_.joinable()) slot_thread_.join();
    if (rpc_) rpc_->stop();
    transport_.stop();
}

void Node::run_forever() {
    install_signal_handlers();
    while (!g_shutdown.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    log_info("apagando...");
    stop();
}

}  // namespace l1::node
