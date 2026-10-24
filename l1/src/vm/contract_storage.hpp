// =============================================================================
// contract_storage.hpp — Storage de contratos
// =============================================================================
#pragma once
#include "../state/state_db.hpp"

namespace l1::vm {

class ContractStorage {
public:
    ContractStorage(state::StateDB& db, const core::Address& contract);
    core::Bytes load(const core::Bytes& key) const;
    void store(const core::Bytes& key, const core::Bytes& value);
    static core::Bytes key_u64(uint64_t k);
private:
    state::StateDB& db_;
    core::Address contract_;
};

}  // namespace l1::vm
