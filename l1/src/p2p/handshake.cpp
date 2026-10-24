// =============================================================================
// handshake.cpp — Handshake autenticado
// =============================================================================
#include "handshake.hpp"

namespace l1::p2p {

HelloMsg make_hello(const core::ChainParams& params, const core::PublicKey& node_id,
                    const std::string& listen_addr, core::BlockHeight tip,
                    const core::Hash256& tip_hash) {
    HelloMsg h;
    h.protocol_version = 1;
    h.chain_id = params.chain_id;
    h.node_id = node_id;
    h.listen_addr = listen_addr;
    h.tip_height = tip;
    h.tip_hash = tip_hash;
    return h;
}

HandshakeResult check_hello(const HelloMsg& remote, const core::ChainParams& local) {
    HandshakeResult r;
    r.remote = remote;
    if (remote.protocol_version != 1) {
        r.error = "protocol";
        return r;
    }
    if (remote.chain_id != local.chain_id) {
        r.error = "chain_id mismatch";
        return r;
    }
    r.ok = true;
    return r;
}

}  // namespace l1::p2p
