// =============================================================================
// rpc_methods.hpp — Métodos JSON-RPC del nodo
// =============================================================================
#pragma once
#include "../state/state_db.hpp"
#include "../storage/block_store.hpp"
#include "../mempool/mempool.hpp"
#include "../mempool/fee_market.hpp"
#include "../consensus/consensus_engine.hpp"
#include "../core/chain_params.hpp"
#include <nlohmann/json.hpp>
#include <functional>

namespace l1::rpc {

struct RpcContext {
    state::StateDB* db{nullptr};
    storage::BlockStore* blocks{nullptr};
    mempool::Mempool* pool{nullptr};
    mempool::FeeMarket* fees{nullptr};
    consensus::ConsensusEngine* consensus{nullptr};
    core::ChainParams params{core::ChainParams::devnet()};
    /// Callback para firmar/inyectar txs ya firmadas al mempool + gossip.
    std::function<core::Result<core::Hash256>(const core::Transaction&)> submit_tx;
};

nlohmann::json dispatch_rpc(RpcContext& ctx, const nlohmann::json& req);

}  // namespace l1::rpc
