// =============================================================================
// state_db.cpp — Lectura/escritura de estado con caché y journal
// =============================================================================
#include "state_db.hpp"

namespace l1::state {

core::Bytes StateDB::addr_key(const core::Address& a) {
    return core::Bytes(a.begin(), a.end());
}

std::optional<core::Account> StateDB::get_account(const core::Address& addr) const {
    auto raw = accounts_.get(addr_key(addr));
    if (!raw) return std::nullopt;
    auto acc = core::Account::decode(*raw);
    if (!acc) return std::nullopt;
    return acc.value();
}

void StateDB::set_account(const core::Address& addr, const core::Account& acc) {
    journal_.push_back(JournalEntry{addr, get_account(addr)});
    accounts_.put(addr_key(addr), acc.encode());
}

void StateDB::kill_account(const core::Address& addr) {
    journal_.push_back(JournalEntry{addr, get_account(addr)});
    accounts_.del(addr_key(addr));
    storage_.erase(addr);
}

core::Result<core::Bytes> StateDB::get_storage(const core::Address& addr,
                                               const core::Bytes& key) const {
    auto it = storage_.find(addr);
    if (it == storage_.end()) return core::Bytes{};
    auto v = it->second.get(key);
    return v.value_or(core::Bytes{});
}

void StateDB::set_storage(const core::Address& addr, const core::Bytes& key,
                          const core::Bytes& value) {
    // Actualiza storage trie y storage_root de la cuenta.
    auto& trie = storage_[addr];
    if (value.empty()) trie.del(key);
    else trie.put(key, value);
    auto acc = get_account(addr).value_or(core::Account{});
    acc.storage_root = trie.root_hash();
    set_account(addr, acc);
}

core::Hash256 StateDB::state_root() const {
    return accounts_.root_hash();
}

size_t StateDB::snapshot() {
    snapshots_.push_back(journal_.size());
    return snapshots_.size() - 1;
}

void StateDB::revert(size_t snap) {
    if (snap >= snapshots_.size()) return;
    size_t target = snapshots_[snap];
    while (journal_.size() > target) {
        auto e = journal_.back();
        journal_.pop_back();
        if (e.prev) accounts_.put(addr_key(e.address), e.prev->encode());
        else accounts_.del(addr_key(e.address));
    }
    snapshots_.resize(snap);
}

void StateDB::commit() {
    journal_.clear();
    snapshots_.clear();
}

void StateDB::apply_alloc(const std::map<core::Address, core::Amount>& alloc) {
    for (const auto& [addr, bal] : alloc) {
        core::Account a;
        a.balance = bal;
        a.nonce = 0;
        set_account(addr, a);
    }
    commit();
}

}  // namespace l1::state
