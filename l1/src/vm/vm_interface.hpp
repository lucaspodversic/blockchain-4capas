// =============================================================================
// vm_interface.hpp — Interfaz de la VM (deploy, call, estimate gas)
// Encaja en: state_transition / executor. Swap-in de wasmtime:
//   implementar WasmtimeVm : VmInterface y registrar en el nodo.
// Por defecto usamos BytecodeVm (subset propio) para evitar dependencia pesada.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"
#include <string>

namespace l1::state { class StateDB; }

namespace l1::vm {

struct VmResult {
    bool success{false};
    core::Gas gas_used{0};
    core::Bytes output;
    std::string error;
};

class VmInterface {
public:
    virtual ~VmInterface() = default;

    virtual core::Result<VmResult> deploy(state::StateDB& db,
                                          const core::Address& contract,
                                          const core::Bytes& code,
                                          const core::Address& caller,
                                          core::Amount value,
                                          core::Gas gas_limit) = 0;

    virtual core::Result<VmResult> call(state::StateDB& db,
                                        const core::Address& contract,
                                        const core::Bytes& input,
                                        const core::Address& caller,
                                        core::Amount value,
                                        core::Gas gas_limit) = 0;

    virtual core::Result<core::Gas> estimate_gas(state::StateDB& db,
                                                 const core::Address& contract,
                                                 const core::Bytes& input,
                                                 const core::Address& caller,
                                                 core::Amount value) = 0;
};

}  // namespace l1::vm
