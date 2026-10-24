// =============================================================================
// mempool.cpp — Almacena, valida y descarta transacciones pendientes
// =============================================================================
#include "mempool.hpp"
#include <algorithm>

namespace l1::mempool {

Mempool::Mempool(core::ChainParams params) : params_(std::move(params)) {}

core::Result<void> Mempool::add(const core::Transaction& tx, core::Amount base_fee) {
    auto st = tx.validate_structure();
    if (!st) return st;
    if (tx.chain_id != params_.chain_id) {
        return {core::ErrorCode::InvalidTransaction, "chain_id"};
    }
    if (tx.max_fee_per_gas < base_fee) {
        return {core::ErrorCode::InsufficientGas, "fee < base"};
    }
    auto h = tx.hash();
    std::lock_guard lock(mu_);
    if (by_hash_.count(h)) return {core::ErrorCode::AlreadyExists, "tx"};
    auto& m = by_sender_[tx.from];
    auto it = m.find(tx.nonce);
    if (it != m.end()) {
        auto& old = by_hash_[it->second];
        if (tx.max_fee_per_gas <= old.max_fee_per_gas) {
            return {core::ErrorCode::InvalidTransaction, "fee too low to replace"};
        }
        by_hash_.erase(it->second);
    }
    by_hash_[h] = tx;
    m[tx.nonce] = h;
    return core::Result<void>::Ok();
}

void Mempool::remove(const core::Hash256& tx_hash) {
    std::lock_guard lock(mu_);
    auto it = by_hash_.find(tx_hash);
    if (it == by_hash_.end()) return;
    by_sender_[it->second.from].erase(it->second.nonce);
    by_hash_.erase(it);
}

void Mempool::remove_included(const std::vector<core::Transaction>& txs) {
    for (const auto& tx : txs) remove(tx.hash());
}

std::vector<core::Transaction> Mempool::select(size_t max, core::Gas gas_limit) const {
    std::lock_guard lock(mu_);
    struct Item { core::Amount fee; core::Transaction tx; };
    std::vector<Item> items;
    for (const auto& [_, tx] : by_hash_) items.push_back({tx.max_fee_per_gas, tx});
    std::sort(items.begin(), items.end(),
              [](const Item& a, const Item& b) { return a.fee > b.fee; });
    std::vector<core::Transaction> out;
    std::map<core::Address, core::Nonce> next_nonce;
    for (const auto& [addr, m] : by_sender_) {
        if (!m.empty()) next_nonce[addr] = m.begin()->first;
    }
    core::Gas gas = 0;
    for (const auto& it : items) {
        if (out.size() >= max) break;
        auto want = next_nonce[it.tx.from];
        if (it.tx.nonce != want) continue;
        if (gas + it.tx.gas_limit > gas_limit) continue;
        out.push_back(it.tx);
        gas += it.tx.gas_limit;
        next_nonce[it.tx.from] = want + 1;
    }
    return out;
}

size_t Mempool::size() const {
    std::lock_guard lock(mu_);
    return by_hash_.size();
}

std::optional<core::Transaction> Mempool::get(const core::Hash256& h) const {
    std::lock_guard lock(mu_);
    auto it = by_hash_.find(h);
    if (it == by_hash_.end()) return std::nullopt;
    return it->second;
}

std::vector<core::Transaction> Mempool::all() const {
    std::lock_guard lock(mu_);
    std::vector<core::Transaction> v;
    for (const auto& [_, tx] : by_hash_) v.push_back(tx);
    return v;
}

}  // namespace l1::mempool
