// =============================================================================
// mempool.hpp — Pool de transacciones pendientes
// Encaja en: RPC sendTx, gossip P2P y proposición de bloques.
// =============================================================================
#pragma once
#include "../core/transaction.hpp"
#include "../core/chain_params.hpp"
#include "../core/error.hpp"
#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <optional>

namespace l1::mempool {

class Mempool {
public:
    explicit Mempool(core::ChainParams params);

    core::Result<void> add(const core::Transaction& tx, core::Amount base_fee);
    void remove(const core::Hash256& tx_hash);
    void remove_included(const std::vector<core::Transaction>& txs);

    /// Selecciona hasta max txs ordenadas por fee efectivo, respetando nonces.
    std::vector<core::Transaction> select(size_t max, core::Gas gas_limit) const;

    size_t size() const;
    std::optional<core::Transaction> get(const core::Hash256& h) const;
    std::vector<core::Transaction> all() const;

private:
    core::ChainParams params_;
    mutable std::mutex mu_;
    std::map<core::Hash256, core::Transaction> by_hash_;
    /// from → nonce → hash
    std::map<core::Address, std::map<core::Nonce, core::Hash256>> by_sender_;
};

}  // namespace l1::mempool
