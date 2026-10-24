// =============================================================================
// main.cpp — Punto de entrada del ejecutable l1d
// =============================================================================
#include "node.hpp"
#include "logger.hpp"
#include <iostream>

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help") {
            std::cout <<
                "l1d — nodo Layer 1\n"
                "  --data-dir PATH\n"
                "  --genesis PATH\n"
                "  --keystore PATH --password PASS\n"
                "  --p2p-port N --rpc-port N --rpc-host ADDR\n"
                "  --bootnode host:port\n"
                "  --validator --rocksdb --api-token TOKEN\n"
                "  --chain-id N --log-level info|debug\n";
            return 0;
        }
    }
    auto cfg = l1::node::NodeConfig::from_args(argc, argv);
    if (cfg.log_level == "debug") {
        l1::node::Logger::instance().set_level(l1::node::LogLevel::Debug);
    }
    l1::node::Node node(cfg);
    auto r = node.start();
    if (!r) {
        l1::node::log_error("start failed: " + r.error().to_string());
        return 1;
    }
    node.run_forever();
    return 0;
}
