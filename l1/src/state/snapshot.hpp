// =============================================================================
// snapshot.hpp — Export/import de snapshots de estado
// =============================================================================
#pragma once
#include "state_db.hpp"

namespace l1::state {

struct StateSnapshot {
    core::Hash256 root{};
    core::Bytes payload;
};

StateSnapshot export_snapshot(const StateDB& db);
core::Result<StateDB> import_snapshot(const StateSnapshot& snap);

}  // namespace l1::state
