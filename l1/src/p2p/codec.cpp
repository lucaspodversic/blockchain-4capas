// =============================================================================
// codec.cpp — Codificación/decodificación de mensajes (len-prefix + type + body)
// Frame: u32 LE length | u8 type | body
// =============================================================================
#include "messages.hpp"
#include "../core/serialize.hpp"
#include "../core/error.hpp"

namespace l1::p2p {

core::Bytes encode_message(const Message& msg) {
    core::Bytes body;
    core::Encoder e(body);
    e.u8(static_cast<uint8_t>(msg.type));
    switch (msg.type) {
        case MsgType::Hello: {
            auto& h = std::get<HelloMsg>(msg.payload);
            e.u32(h.protocol_version);
            e.u64(h.chain_id);
            e.fixed(h.node_id);
            e.str(h.listen_addr);
            e.u64(h.tip_height);
            e.fixed(h.tip_hash);
            break;
        }
        case MsgType::Ping:
        case MsgType::Pong: {
            auto nonce = msg.type == MsgType::Ping
                ? std::get<PingMsg>(msg.payload).nonce
                : std::get<PongMsg>(msg.payload).nonce;
            e.u64(nonce);
            break;
        }
        case MsgType::Tx: {
            auto raw = std::get<core::Transaction>(msg.payload).encode();
            e.bytes_len(raw);
            break;
        }
        case MsgType::Block: {
            auto raw = std::get<core::Block>(msg.payload).encode();
            e.bytes_len(raw);
            break;
        }
        case MsgType::Vote: {
            auto& v = std::get<core::Vote>(msg.payload);
            e.fixed(v.block_hash);
            e.u64(v.slot);
            e.fixed(v.validator);
            e.fixed(v.signature);
            break;
        }
        case MsgType::GetBlocks: {
            auto& g = std::get<GetBlocksMsg>(msg.payload);
            e.u64(g.from);
            e.u64(g.to);
            break;
        }
        case MsgType::Blocks: {
            auto& b = std::get<BlocksMsg>(msg.payload);
            e.u32(static_cast<uint32_t>(b.blocks.size()));
            for (const auto& blk : b.blocks) {
                auto raw = blk.encode();
                e.bytes_len(raw);
            }
            break;
        }
        default:
            break;
    }
    core::Bytes frame;
    core::Encoder fe(frame);
    fe.u32(static_cast<uint32_t>(body.size()));
    fe.bytes(body);
    return frame;
}

core::Result<Message> decode_message(const core::Bytes& frame) {
    core::Decoder d(frame);
    auto len = d.u32();
    if (!len) return len.error();
    auto body = d.bytes(len.value());
    if (!body) return body.error();
    core::Decoder bd(body.value());
    auto t = bd.u8();
    if (!t) return t.error();
    Message msg;
    msg.type = static_cast<MsgType>(t.value());
    switch (msg.type) {
        case MsgType::Hello: {
            HelloMsg h;
            auto pv = bd.u32(); if (!pv) return pv.error();
            auto cid = bd.u64(); if (!cid) return cid.error();
            auto nid = bd.fixed<32>(); if (!nid) return nid.error();
            auto addr = bd.str(); if (!addr) return addr.error();
            auto th = bd.u64(); if (!th) return th.error();
            auto tip = bd.fixed<32>(); if (!tip) return tip.error();
            h.protocol_version = pv.value();
            h.chain_id = cid.value();
            h.node_id = nid.value();
            h.listen_addr = addr.value();
            h.tip_height = th.value();
            h.tip_hash = tip.value();
            msg.payload = h;
            break;
        }
        case MsgType::Ping: {
            auto n = bd.u64(); if (!n) return n.error();
            msg.payload = PingMsg{n.value()};
            break;
        }
        case MsgType::Pong: {
            auto n = bd.u64(); if (!n) return n.error();
            msg.payload = PongMsg{n.value()};
            break;
        }
        case MsgType::Tx: {
            auto raw = bd.bytes_len(); if (!raw) return raw.error();
            auto tx = core::Transaction::decode(raw.value());
            if (!tx) return tx.error();
            msg.payload = tx.value();
            break;
        }
        case MsgType::Block: {
            auto raw = bd.bytes_len(); if (!raw) return raw.error();
            auto b = core::Block::decode(raw.value());
            if (!b) return b.error();
            msg.payload = b.value();
            break;
        }
        case MsgType::Vote: {
            core::Vote v;
            auto bh = bd.fixed<32>(); if (!bh) return bh.error();
            auto slot = bd.u64(); if (!slot) return slot.error();
            auto val = bd.fixed<20>(); if (!val) return val.error();
            auto sig = bd.fixed<64>(); if (!sig) return sig.error();
            v.block_hash = bh.value();
            v.slot = slot.value();
            v.validator = val.value();
            v.signature = sig.value();
            msg.payload = v;
            break;
        }
        case MsgType::GetBlocks: {
            GetBlocksMsg g;
            auto f = bd.u64(); if (!f) return f.error();
            auto t2 = bd.u64(); if (!t2) return t2.error();
            g.from = f.value();
            g.to = t2.value();
            msg.payload = g;
            break;
        }
        case MsgType::Blocks: {
            BlocksMsg bm;
            auto n = bd.u32(); if (!n) return n.error();
            for (uint32_t i = 0; i < n.value(); ++i) {
                auto raw = bd.bytes_len(); if (!raw) return raw.error();
                auto b = core::Block::decode(raw.value());
                if (!b) return b.error();
                bm.blocks.push_back(b.value());
            }
            msg.payload = bm;
            break;
        }
        default:
            return {core::ErrorCode::DeserializationError, "unknown msg"};
    }
    return msg;
}

}  // namespace l1::p2p
