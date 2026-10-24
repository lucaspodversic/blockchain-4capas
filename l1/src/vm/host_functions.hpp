// =============================================================================
// host_functions.hpp — Host ABI para la VM
// =============================================================================
#pragma once
#include "gas_meter.hpp"
#include "../state/state_db.hpp"
#include "../crypto/hash.hpp"

namespace l1::vm {

struct HostContext {
    state::StateDB& db;
    core::Address contract;
    core::Address caller;
    core::Amount value;
    GasMeter& gas;
};

core::Result<core::Bytes> host_storage_load(HostContext& ctx, const core::Bytes& key);
core::Result<void> host_storage_store(HostContext& ctx, const core::Bytes& key,
                                      const core::Bytes& value);
core::Result<core::Hash256> host_keccak(HostContext& ctx, const core::Bytes& data);
core::Result<void> host_transfer(HostContext& ctx, const core::Address& to, core::Amount amount);

}  // namespace l1::vm
