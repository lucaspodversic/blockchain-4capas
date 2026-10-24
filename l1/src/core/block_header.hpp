// =============================================================================
// block_header.hpp — Cabecera de bloque (raíces Merkle + metadatos)
// Encaja en: consenso, fork choice, sync y almacenamiento de bloques.
// =============================================================================
#pragma once

#include "types.hpp"
#include "serialize.hpp"
#include "error.hpp"

namespace l1::core {

struct BlockHeader {
    BlockHeight height{0};
    Hash256 parent_hash{zero_hash()};
    Hash256 state_root{zero_hash()};
    Hash256 tx_root{zero_hash()};       ///< Merkle root de las txs del bloque
    Hash256 receipts_root{zero_hash()};
    Address proposer{zero_address()};
    uint64_t timestamp{0};              ///< Unix seconds
    Gas gas_limit{0};
    Gas gas_used{0};
    Amount base_fee{0};
    Slot slot{0};
    Epoch epoch{0};
    Bytes extra_data;

    Bytes encode() const {
        Bytes out;
        Encoder e(out);
        e.u64(height);
        e.fixed(parent_hash);
        e.fixed(state_root);
        e.fixed(tx_root);
        e.fixed(receipts_root);
        e.fixed(proposer);
        e.u64(timestamp);
        e.u64(gas_limit);
        e.u64(gas_used);
        e.u64(base_fee);
        e.u64(slot);
        e.u64(epoch);
        e.bytes_len(extra_data);
        return out;
    }

    static Result<BlockHeader> decode(const Bytes& raw) {
        Decoder d(raw);
        BlockHeader h;
        auto height = d.u64(); if (!height) return height.error();
        auto parent = d.fixed<32>(); if (!parent) return parent.error();
        auto state = d.fixed<32>(); if (!state) return state.error();
        auto txr = d.fixed<32>(); if (!txr) return txr.error();
        auto rr = d.fixed<32>(); if (!rr) return rr.error();
        auto prop = d.fixed<20>(); if (!prop) return prop.error();
        auto ts = d.u64(); if (!ts) return ts.error();
        auto gl = d.u64(); if (!gl) return gl.error();
        auto gu = d.u64(); if (!gu) return gu.error();
        auto bf = d.u64(); if (!bf) return bf.error();
        auto slot = d.u64(); if (!slot) return slot.error();
        auto ep = d.u64(); if (!ep) return ep.error();
        auto extra = d.bytes_len(); if (!extra) return extra.error();
        h.height = height.value();
        h.parent_hash = parent.value();
        h.state_root = state.value();
        h.tx_root = txr.value();
        h.receipts_root = rr.value();
        h.proposer = prop.value();
        h.timestamp = ts.value();
        h.gas_limit = gl.value();
        h.gas_used = gu.value();
        h.base_fee = bf.value();
        h.slot = slot.value();
        h.epoch = ep.value();
        h.extra_data = extra.value();
        return h;
    }

    Hash256 hash() const;  // definido en block.cpp (usa keccak)
};

}  // namespace l1::core
