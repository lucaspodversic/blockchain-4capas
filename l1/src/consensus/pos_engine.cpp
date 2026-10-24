// =============================================================================
// pos_engine.cpp — Motor PoS completo (proponer / votar / finalizar)
// =============================================================================
#include "pos_engine.hpp"
#include "../crypto/ed25519.hpp"
#include "../crypto/address.hpp"
#include "../mempool/fee_market.hpp"
#include <ctime>

namespace l1::consensus {

PosEngine::PosEngine(PosConfig cfg, ValidatorSet vs, state::StateDB& db, mempool::Mempool& pool)
    : cfg_(std::move(cfg)),
      vs_(std::move(vs)),
      ps_(vs_),
      bval_(cfg_.params, vs_, ps_),
      finality_(vs_, cfg_.params.finality_threshold_bps),
      epochs_(cfg_.params, vs_),
      db_(db),
      pool_(pool) {}

void PosEngine::set_genesis(const core::Block& genesis) {
    fc_.add_block(genesis);
    finality_.set_justified(genesis.hash());
    finality_.set_finalized(genesis.hash());
    last_header_ = genesis.header;
    has_parent_ = true;
}

core::Result<core::Block> PosEngine::propose(core::Slot slot) {
    core::Block b;
    b.header.height = has_parent_ ? last_header_.height + 1 : 0;
    b.header.parent_hash = has_parent_ ? last_header_.hash() : core::zero_hash();
    b.header.proposer = cfg_.local_address;
    b.header.timestamp = static_cast<uint64_t>(std::time(nullptr));
    b.header.gas_limit = cfg_.params.block_gas_limit;
    b.header.base_fee = cfg_.params.initial_base_fee;
    b.header.slot = slot;
    b.header.epoch = epochs_.epoch_of(slot);
    b.transactions = pool_.select(1000, cfg_.params.block_gas_limit);
    b.compute_tx_root();

    // Ejecutar para obtener state_root.
    state::BlockExecutor exec(db_, cfg_.params, nullptr);
    auto er = exec.execute(b);
    if (!er) return er.error();
    b.header.state_root = er.value().state_root;
    b.header.gas_used = er.value().gas_used;

    auto h = b.hash();
    b.proposer_signature = crypto::ed25519_sign(cfg_.local_sk, h);
    b.has_proposer_sig = true;
    return b;
}

core::Result<std::vector<ConsensusEvent>> PosEngine::on_slot(core::Slot slot) {
    std::vector<ConsensusEvent> evs;
    auto ep = epochs_.epoch_of(slot);
    if (slot > 0 && slot % cfg_.params.slots_per_epoch == 0) {
        epochs_.on_epoch_transition(ep);
    }
    auto proposer = ps_.select(slot, epochs_.epoch_seed());
    if (cfg_.is_validator && proposer == cfg_.local_address) {
        auto block = propose(slot);
        if (!block) return block.error();
        auto r = on_block(block.value());
        if (!r) return r;
        ConsensusEvent e;
        e.kind = ConsensusEvent::Kind::Proposed;
        e.block = block.value();
        e.block_hash = block.value().hash();
        evs.push_back(e);
        for (auto& x : r.value()) evs.push_back(std::move(x));
        // Auto-voto
        core::Vote v;
        v.block_hash = e.block_hash;
        v.slot = slot;
        v.validator = cfg_.local_address;
        v.signature = crypto::ed25519_sign(cfg_.local_sk, e.block_hash);
        auto vr = on_vote(v);
        if (vr) for (auto& x : vr.value()) evs.push_back(std::move(x));
    }
    return evs;
}

core::Result<std::vector<ConsensusEvent>> PosEngine::on_block(const core::Block& block) {
    std::vector<ConsensusEvent> evs;
    if (has_parent_ && block.header.height > 0) {
        auto val = bval_.validate(block, last_header_, epochs_.epoch_seed());
        if (!val) return val.error();
    }
    fc_.add_block(block);
    // Actualizar tip local si es extensión de nuestra cabecera.
    if (!has_parent_ || block.header.parent_hash == last_header_.hash() ||
        block.header.height == 0) {
        last_header_ = block.header;
        has_parent_ = true;
    } else {
        // Reorg posible: seguir fork choice.
        auto tip = fc_.head();
        auto blk = fc_.get(tip);
        if (blk) last_header_ = blk->header;
    }
    pool_.remove_included(block.transactions);
    return evs;
}

core::Result<std::vector<ConsensusEvent>> PosEngine::on_vote(const core::Vote& vote) {
    std::vector<ConsensusEvent> evs;
    if (auto ev = slash_.check(vote)) {
        return {core::ErrorCode::ConsensusFailure, ev->reason};
    }
    auto val = vs_.get(vote.validator);
    if (!val || !val->active) return {core::ErrorCode::Unauthorized, "voter"};
    // Verificar firma
    if (!crypto::ed25519_verify(val->pubkey, vote.block_hash, vote.signature)) {
        return {core::ErrorCode::InvalidSignature, "vote"};
    }
    fc_.add_vote(vote, val->stake);
    bool fin = finality_.on_vote(vote, val->stake);
    ConsensusEvent e;
    e.kind = ConsensusEvent::Kind::Voted;
    e.block_hash = vote.block_hash;
    evs.push_back(e);
    if (fin) {
        ConsensusEvent f;
        f.kind = ConsensusEvent::Kind::Finalized;
        f.block_hash = finality_.finalized();
        evs.push_back(f);
    }
    return evs;
}

core::Hash256 PosEngine::head() const { return fc_.head(); }
core::Hash256 PosEngine::finalized() const { return finality_.finalized(); }
core::BlockHeight PosEngine::height() const {
    auto h = fc_.head();
    return fc_.height_of(h);
}

}  // namespace l1::consensus
