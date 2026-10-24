// =============================================================================
// config.hpp — Configuración del nodo (CLI + JSON simple)
// =============================================================================
#pragma once
#include "../core/chain_params.hpp"
#include <string>
#include <vector>

namespace l1::node {

struct NodeConfig {
    core::ChainParams params{core::ChainParams::devnet()};
    std::string data_dir{"./data"};
    std::string genesis_path{"genesis.json"};
    std::string keystore_path;
    std::string keystore_password;
    std::string listen_p2p{"0.0.0.0"};
    uint16_t p2p_port{30303};
    std::string listen_rpc{"127.0.0.1"};
    uint16_t rpc_port{8545};
    std::string api_token;
    std::vector<std::string> bootnodes;  // host:port
    bool validator{false};
    bool use_rocksdb{false};
    std::string log_level{"info"};

    static NodeConfig from_args(int argc, char** argv);
};

}  // namespace l1::node
