// =============================================================================
// gas_meter.cpp — Medición y cobro de gas por instrucción
// =============================================================================
#include "gas_meter.hpp"

namespace l1::vm {

GasMeter::GasMeter(core::Gas limit) : limit_(limit), used_(0) {}

core::Result<void> GasMeter::charge(core::Gas cost) {
    if (used_ + cost > limit_) return {core::ErrorCode::VmOutOfGas, "out of gas"};
    used_ += cost;
    return core::Result<void>::Ok();
}

core::Gas GasMeter::used() const { return used_; }
core::Gas GasMeter::remaining() const { return limit_ - used_; }
core::Gas GasMeter::limit() const { return limit_; }

}  // namespace l1::vm
