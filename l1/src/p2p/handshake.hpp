// =============================================================================
// handshake.hpp — Handshake autenticado entre nodos (Hello / HelloAck)
// =============================================================================
#pragma once
#include "messages.hpp"
#include "../crypto/ed25519.hpp"
#include "../core/chain_params.hpp"

namespace l1::p2p {

struct HandshakeResult {
    bool ok{false};
    HelloMsg remote{};
    std::string error;
};

HelloMsg make_hello(const core::ChainParams& params, const core::PublicKey& node_id,
                    const std::string& listen_addr, core::BlockHeight tip,
                    const core::Hash256& tip_hash);

HandshakeResult check_hello(const HelloMsg& remote, const core::ChainParams& local);

}  // namespace l1::p2p
