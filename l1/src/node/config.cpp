// =============================================================================
// config.cpp — Lectura de configuración CLI
// =============================================================================
#include "config.hpp"
#include <cstring>

namespace l1::node {

static bool eq(const char* a, const char* b) { return std::strcmp(a, b) == 0; }

NodeConfig NodeConfig::from_args(int argc, char** argv) {
    NodeConfig c;
    for (int i = 1; i < argc; ++i) {
        if (eq(argv[i], "--data-dir") && i + 1 < argc) c.data_dir = argv[++i];
        else if (eq(argv[i], "--genesis") && i + 1 < argc) c.genesis_path = argv[++i];
        else if (eq(argv[i], "--keystore") && i + 1 < argc) c.keystore_path = argv[++i];
        else if (eq(argv[i], "--password") && i + 1 < argc) c.keystore_password = argv[++i];
        else if (eq(argv[i], "--p2p-port") && i + 1 < argc) c.p2p_port = static_cast<uint16_t>(std::stoi(argv[++i]));
        else if (eq(argv[i], "--rpc-port") && i + 1 < argc) c.rpc_port = static_cast<uint16_t>(std::stoi(argv[++i]));
        else if (eq(argv[i], "--rpc-host") && i + 1 < argc) c.listen_rpc = argv[++i];
        else if (eq(argv[i], "--bootnode") && i + 1 < argc) c.bootnodes.push_back(argv[++i]);
        else if (eq(argv[i], "--validator")) c.validator = true;
        else if (eq(argv[i], "--rocksdb")) c.use_rocksdb = true;
        else if (eq(argv[i], "--api-token") && i + 1 < argc) c.api_token = argv[++i];
        else if (eq(argv[i], "--chain-id") && i + 1 < argc) c.params.chain_id = std::stoull(argv[++i]);
        else if (eq(argv[i], "--log-level") && i + 1 < argc) c.log_level = argv[++i];
        else if (eq(argv[i], "--help")) {
            // handled by main
        }
    }
    return c;
}

}  // namespace l1::node
