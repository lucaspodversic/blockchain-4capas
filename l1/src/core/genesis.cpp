// =============================================================================
// genesis.cpp — Construcción del génesis y carga desde JSON
// =============================================================================
#include "genesis.hpp"
#include "../crypto/hash.hpp"
#include <nlohmann/json.hpp>
#include <ctime>

namespace l1::core {

using json = nlohmann::json;

Block GenesisConfig::build_block() const {
    Block b;
    b.header.height = 0;
    b.header.parent_hash = zero_hash();
    b.header.timestamp = timestamp ? timestamp : static_cast<uint64_t>(std::time(nullptr));
    b.header.gas_limit = params.block_gas_limit;
    b.header.gas_used = 0;
    b.header.base_fee = params.initial_base_fee;
    b.header.slot = 0;
    b.header.epoch = 0;
    b.header.extra_data = extra_data;
    // state_root se calcula al aplicar alloc en StateDB; aquí placeholder.
    b.header.state_root = zero_hash();
    b.compute_tx_root();
    return b;
}

std::string GenesisConfig::to_json() const {
    json j;
    j["chain_id"] = params.chain_id;
    j["network_name"] = params.network_name;
    j["timestamp"] = timestamp;
    j["slot_ms"] = params.slot_ms;
    j["block_gas_limit"] = params.block_gas_limit;
    j["block_reward"] = params.block_reward;
    j["initial_base_fee"] = params.initial_base_fee;
    j["extra_data"] = to_hex(extra_data);
    json alloc_j = json::object();
    for (const auto& [addr, bal] : alloc) {
        alloc_j[to_hex(addr)] = bal;
    }
    j["alloc"] = alloc_j;
    json vals = json::array();
    for (const auto& v : validators) {
        vals.push_back({
            {"address", to_hex(v.address)},
            {"pubkey", to_hex(v.pubkey)},
            {"stake", v.stake},
        });
    }
    j["validators"] = vals;
    return j.dump(2);
}

Result<GenesisConfig> GenesisConfig::from_json(const std::string& raw) {
    try {
        auto j = json::parse(raw);
        GenesisConfig g;
        g.params.chain_id = j.value("chain_id", 1337ULL);
        g.params.network_name = j.value("network_name", std::string("l1-devnet"));
        g.timestamp = j.value("timestamp", 0ULL);
        g.params.slot_ms = j.value("slot_ms", 1000ULL);
        g.params.block_gas_limit = j.value("block_gas_limit", 30'000'000ULL);
        g.params.block_reward = j.value("block_reward", 2'000'000'000ULL);
        g.params.initial_base_fee = j.value("initial_base_fee", 1ULL);
        if (j.contains("extra_data")) {
            g.extra_data = from_hex(j["extra_data"].get<std::string>());
        }
        if (j.contains("alloc")) {
            for (auto it = j["alloc"].begin(); it != j["alloc"].end(); ++it) {
                auto addr = from_hex_arr<20>(it.key());
                if (!addr) return {ErrorCode::ConfigError, "alloc address inválida"};
                g.alloc[*addr] = it.value().get<Amount>();
            }
        }
        if (j.contains("validators")) {
            for (const auto& vj : j["validators"]) {
                GenesisValidator v;
                auto a = from_hex_arr<20>(vj.at("address").get<std::string>());
                auto pk = from_hex_arr<32>(vj.at("pubkey").get<std::string>());
                if (!a || !pk) return {ErrorCode::ConfigError, "validator inválido"};
                v.address = *a;
                v.pubkey = *pk;
                v.stake = vj.value("stake", 0ULL);
                g.validators.push_back(v);
            }
        }
        return g;
    } catch (const std::exception& ex) {
        return {ErrorCode::ConfigError, ex.what()};
    }
}

}  // namespace l1::core
