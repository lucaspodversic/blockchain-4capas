// =============================================================================
// state_db.hpp — Base de estado: cuentas + storage por contrato + journal
// Encaja en: transiciones de estado y executor de bloques.
// =============================================================================
#pragma once
#include "../core/account.hpp"
#include "../core/types.hpp"
#include "../core/error.hpp"
#include "trie.hpp"
#include <map>
#include <vector>
#include <optional>

namespace l1::state {

struct JournalEntry {
    core::Address address{};
    std::optional<core::Account> prev;  ///< nullopt = cuenta no existía
};

class StateDB {
public:
    StateDB() = default;

    std::optional<core::Account> get_account(const core::Address& addr) const;
    void set_account(const core::Address& addr, const core::Account& acc);
    void kill_account(const core::Address& addr);

    core::Result<core::Bytes> get_storage(const core::Address& addr, const core::Bytes& key) const;
    void set_storage(const core::Address& addr, const core::Bytes& key, const core::Bytes& value);

    core::Hash256 state_root() const;

    /// Checkpoint para rollback de tx fallida.
    size_t snapshot();
    void revert(size_t snap);
    void commit();  ///< Descarta journal (cambios ya aplicados en memoria)

    /// Aplica alloc del génesis.
    void apply_alloc(const std::map<core::Address, core::Amount>& alloc);

    const Trie& account_trie() const { return accounts_; }

private:
    static core::Bytes addr_key(const core::Address& a);

    Trie accounts_;
    std::map<core::Address, Trie> storage_;
    std::vector<JournalEntry> journal_;
    std::vector<size_t> snapshots_;
};

}  // namespace l1::state
