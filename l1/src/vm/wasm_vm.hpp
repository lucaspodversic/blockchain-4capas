// =============================================================================
// wasm_vm.hpp — Factory de la VM por defecto (bytecode L1BC)
// =============================================================================
#pragma once
#include "vm_interface.hpp"
#include <memory>

namespace l1::vm {
std::unique_ptr<VmInterface> create_default_vm();
}  // namespace l1::vm
