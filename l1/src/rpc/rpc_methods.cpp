// =============================================================================
// rpc_methods.cpp — getBalance, sendTx, getBlock, call, etc.
// =============================================================================
#include "rpc_methods.hpp"
#include "../core/serialize.hpp"
#include "../core/transaction.hpp"

namespace l1::rpc {

using json = nlohmann::json;

static json ok(const json& id, json result) {
    return json{{"jsonrpc", "2.0"}, {"id", id}, {"result", std::move(result)}};
}
static json err(const json& id, int code, const std::string& msg) {
    return json{{"jsonrpc", "2.0"}, {"id", id},
                {"error", {{"code", code}, {"message", msg}}}};
}

json dispatch_rpc(RpcContext& ctx, const json& req) {
    json id = req.value("id", json(nullptr));
    if (!req.contains("method")) return err(id, -32600, "no method");
    std::string method = req["method"].get<std::string>();
    json params = req.value("params", json::array());

    try {
        if (method == "l1_chainId" || method == "eth_chainId") {
            return ok(id, core::to_hex(core::encode_u64(ctx.params.chain_id)));
        }
        if (method == "l1_blockNumber" || method == "eth_blockNumber") {
            auto h = ctx.consensus ? ctx.consensus->height() : 0;
            return ok(id, h);
        }
        if (method == "l1_getBalance" || method == "eth_getBalance") {
            auto addr = core::from_hex_arr<20>(params.at(0).get<std::string>());
            if (!addr) return err(id, -32602, "bad address");
            auto acc = ctx.db->get_account(*addr);
            return ok(id, acc ? acc->balance : 0);
        }
        if (method == "l1_getBlockByNumber" || method == "eth_getBlockByNumber") {
            core::BlockHeight h = 0;
            if (params.at(0).is_string()) {
                auto s = params.at(0).get<std::string>();
                if (s == "latest") h = ctx.consensus ? ctx.consensus->height() : 0;
                else {
                    auto b = core::from_hex(s);
                    for (auto x : b) h = (h << 8) | x;
                }
            } else h = params.at(0).get<uint64_t>();
            auto blk = ctx.blocks->get_by_height(h);
            if (!blk || !blk.value()) return ok(id, nullptr);
            const auto& b = *blk.value();
            json j;
            j["hash"] = core::to_hex(b.hash());
            j["number"] = b.header.height;
            j["parentHash"] = core::to_hex(b.header.parent_hash);
            j["stateRoot"] = core::to_hex(b.header.state_root);
            j["transactionsRoot"] = core::to_hex(b.header.tx_root);
            j["timestamp"] = b.header.timestamp;
            j["gasUsed"] = b.header.gas_used;
            j["gasLimit"] = b.header.gas_limit;
            j["baseFeePerGas"] = b.header.base_fee;
            j["miner"] = core::to_hex(b.header.proposer);
            j["transactions"] = json::array();
            for (const auto& tx : b.transactions) j["transactions"].push_back(core::to_hex(tx.hash()));
            return ok(id, j);
        }
        if (method == "l1_sendRawTransaction" || method == "eth_sendRawTransaction") {
            auto raw = core::from_hex(params.at(0).get<std::string>());
            auto tx = core::Transaction::decode(raw);
            if (!tx) return err(id, -32000, tx.error().to_string());
            if (ctx.submit_tx) {
                auto h = ctx.submit_tx(tx.value());
                if (!h) return err(id, -32000, h.error().to_string());
                return ok(id, core::to_hex(h.value()));
            }
            auto r = ctx.pool->add(tx.value(), ctx.fees ? ctx.fees->base_fee() : 0);
            if (!r) return err(id, -32000, r.error().to_string());
            return ok(id, core::to_hex(tx.value().hash()));
        }
        if (method == "l1_gasPrice") {
            return ok(id, ctx.fees ? ctx.fees->base_fee() : ctx.params.initial_base_fee);
        }
        if (method == "l1_getTransactionCount") {
            auto addr = core::from_hex_arr<20>(params.at(0).get<std::string>());
            if (!addr) return err(id, -32602, "bad address");
            auto acc = ctx.db->get_account(*addr);
            return ok(id, acc ? acc->nonce : 0);
        }
        return err(id, -32601, "method not found: " + method);
    } catch (const std::exception& ex) {
        return err(id, -32603, ex.what());
    }
}

}  // namespace l1::rpc
