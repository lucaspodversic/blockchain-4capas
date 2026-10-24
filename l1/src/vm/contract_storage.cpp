// =============================================================================
// contract_storage.cpp — Almacenamiento persistente por contrato
// =============================================================================
#include "contract_storage.hpp"
#include "../core/serialize.hpp"

namespace l1::vm {

ContractStorage::ContractStorage(state::StateDB& db, const core::Address& contract)
    : db_(db), contract_(contract) {}

core::Bytes ContractStorage::load(const core::Bytes& key) const {
    auto r = db_.get_storage(contract_, key);
    return r.ok() ? r.value() : core::Bytes{};
}

void ContractStorage::store(const core::Bytes& key, const core::Bytes& value) {
    db_.set_storage(contract_, key, value);
}

core::Bytes ContractStorage::key_u64(uint64_t k) {
    core::Bytes b;
    core::Encoder e(b);
    e.u64(k);
    return b;
}

}  // namespace l1::vm
