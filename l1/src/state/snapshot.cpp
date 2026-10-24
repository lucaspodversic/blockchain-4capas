// =============================================================================
// snapshot.cpp — Snapshots del estado para sync rápido y rollback
// =============================================================================
#include "snapshot.hpp"
#include "../core/serialize.hpp"
#include <cstring>

namespace l1::state {

StateSnapshot export_snapshot(const StateDB& db) {
    StateSnapshot s;
    s.root = db.state_root();
    core::Encoder e(s.payload);
    const auto& entries = db.account_trie().entries();
    e.u32(static_cast<uint32_t>(entries.size()));
    for (const auto& [k, v] : entries) {
        e.bytes_len(k);
        e.bytes_len(v);
    }
    return s;
}

core::Result<StateDB> import_snapshot(const StateSnapshot& snap) {
    StateDB db;
    core::Decoder d(snap.payload);
    auto n = d.u32();
    if (!n) return n.error();
    for (uint32_t i = 0; i < n.value(); ++i) {
        auto k = d.bytes_len(); if (!k) return k.error();
        auto v = d.bytes_len(); if (!v) return v.error();
        if (k.value().size() != 20) return {core::ErrorCode::DeserializationError, "addr"};
        core::Address addr{};
        std::memcpy(addr.data(), k.value().data(), 20);
        auto acc = core::Account::decode(v.value());
        if (!acc) return acc.error();
        db.set_account(addr, acc.value());
    }
    db.commit();
    return db;
}

}  // namespace l1::state
