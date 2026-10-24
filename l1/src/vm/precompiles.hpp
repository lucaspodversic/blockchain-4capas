// =============================================================================
// precompiles.hpp — API de precompiles
// =============================================================================
#pragma once
#include "vm_interface.hpp"

namespace l1::vm {
enum class Precompile : uint8_t { Sha256 = 1, Keccak256 = 2, Ed25519Verify = 3 };
core::Address precompile_address(Precompile p);
bool is_precompile(const core::Address& a);
core::Result<VmResult> run_precompile(const core::Address& addr, const core::Bytes& input,
                                      core::Gas gas_limit);
}  // namespace l1::vm
