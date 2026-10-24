// =============================================================================
// gas_meter.hpp — Contador de gas
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"

namespace l1::vm {

class GasMeter {
public:
    explicit GasMeter(core::Gas limit);
    core::Result<void> charge(core::Gas cost);
    core::Gas used() const;
    core::Gas remaining() const;
    core::Gas limit() const;
private:
    core::Gas limit_;
    core::Gas used_;
};

constexpr core::Gas GAS_BASE = 1;
constexpr core::Gas GAS_STORAGE_LOAD = 200;
constexpr core::Gas GAS_STORAGE_STORE = 5000;
constexpr core::Gas GAS_CALL = 700;
constexpr core::Gas GAS_HASH = 30;
constexpr core::Gas GAS_TRANSFER = 9000;

}  // namespace l1::vm
