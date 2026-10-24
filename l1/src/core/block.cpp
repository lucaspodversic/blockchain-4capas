// =============================================================================
// block.cpp — Hash de cabecera/bloque, tx root Merkle y verificación
// =============================================================================
#include "block.hpp"
#include "../crypto/hash.hpp"
#include "../crypto/merkle_tree.hpp"

namespace l1::core {

Hash256 BlockHeader::hash() const {
    return crypto::keccak256(encode());
}

Bytes Block::encode() const {
    Bytes out;
    Encoder e(out);
    auto hb = header.encode();
    e.bytes_len(hb);
    e.u32(static_cast<uint32_t>(transactions.size()));
    for (const auto& tx : transactions) {
        auto tb = tx.encode();
        e.bytes_len(tb);
    }
    e.u32(static_cast<uint32_t>(votes.size()));
    for (const auto& v : votes) {
        e.fixed(v.block_hash);
        e.u64(v.slot);
        e.fixed(v.validator);
        e.fixed(v.signature);
    }
    e.u8(has_proposer_sig ? 1 : 0);
    if (has_proposer_sig) e.fixed(proposer_signature);
    return out;
}

Result<Block> Block::decode(const Bytes& raw) {
    Decoder d(raw);
    Block b;
    auto hb = d.bytes_len(); if (!hb) return hb.error();
    auto header = BlockHeader::decode(hb.value()); if (!header) return header.error();
    b.header = header.value();
    auto ntx = d.u32(); if (!ntx) return ntx.error();
    b.transactions.reserve(ntx.value());
    for (uint32_t i = 0; i < ntx.value(); ++i) {
        auto tb = d.bytes_len(); if (!tb) return tb.error();
        auto tx = Transaction::decode(tb.value()); if (!tx) return tx.error();
        b.transactions.push_back(tx.value());
    }
    auto nv = d.u32(); if (!nv) return nv.error();
    b.votes.reserve(nv.value());
    for (uint32_t i = 0; i < nv.value(); ++i) {
        Vote v;
        auto bh = d.fixed<32>(); if (!bh) return bh.error();
        auto slot = d.u64(); if (!slot) return slot.error();
        auto val = d.fixed<20>(); if (!val) return val.error();
        auto sig = d.fixed<64>(); if (!sig) return sig.error();
        v.block_hash = bh.value();
        v.slot = slot.value();
        v.validator = val.value();
        v.signature = sig.value();
        b.votes.push_back(v);
    }
    auto hs = d.u8();
    if (hs && hs.value() == 1) {
        auto sig = d.fixed<64>(); if (!sig) return sig.error();
        b.proposer_signature = sig.value();
        b.has_proposer_sig = true;
    }
    return b;
}

Hash256 Block::hash() const {
    return header.hash();
}

void Block::compute_tx_root() {
    std::vector<Hash256> leaves;
    leaves.reserve(transactions.size());
    for (const auto& tx : transactions) {
        leaves.push_back(tx.hash());
    }
    header.tx_root = crypto::MerkleTree::root(leaves);
}

Result<void> Block::validate_integrity() const {
    if (header.gas_used > header.gas_limit) {
        return {ErrorCode::InvalidBlock, "gas_used > gas_limit"};
    }
    // Verificar tx_root
    std::vector<Hash256> leaves;
    for (const auto& tx : transactions) leaves.push_back(tx.hash());
    auto expected = crypto::MerkleTree::root(leaves);
    if (expected != header.tx_root) {
        return {ErrorCode::InvalidBlock, "tx_root mismatch"};
    }
    for (const auto& tx : transactions) {
        auto r = tx.validate_structure();
        if (!r) return r;
    }
    return Result<void>::Ok();
}

}  // namespace l1::core
