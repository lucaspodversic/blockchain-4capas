// =============================================================================
// messages.hpp — Mensajes de red (tx, bloque, voto, ping, sync)
// Encaja en: codec, gossip y sync.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/transaction.hpp"
#include "../core/block.hpp"
#include <variant>
#include <string>

namespace l1::p2p {

enum class MsgType : uint8_t {
    Hello = 1,
    HelloAck = 2,
    Ping = 3,
    Pong = 4,
    Tx = 10,
    Block = 11,
    Vote = 12,
    GetBlocks = 20,
    Blocks = 21,
    Disconnect = 255,
};

struct HelloMsg {
    uint32_t protocol_version{1};
    core::ChainId chain_id{1337};
    core::PublicKey node_id{};
    std::string listen_addr;
    core::BlockHeight tip_height{0};
    core::Hash256 tip_hash{core::zero_hash()};
};

struct PingMsg { uint64_t nonce{0}; };
struct PongMsg { uint64_t nonce{0}; };

struct GetBlocksMsg {
    core::BlockHeight from{0};
    core::BlockHeight to{0};
};

struct BlocksMsg {
    std::vector<core::Block> blocks;
};

using Payload = std::variant<
    HelloMsg, PingMsg, PongMsg,
    core::Transaction, core::Block, core::Vote,
    GetBlocksMsg, BlocksMsg
>;

struct Message {
    MsgType type{MsgType::Ping};
    Payload payload;
};

}  // namespace l1::p2p
