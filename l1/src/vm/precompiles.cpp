// =============================================================================
// precompiles.cpp — Contratos nativos rápidos (hashes, verificación de firmas)
// Direcciones fijas 0x01..0x04 estilo Ethereum.
// =============================================================================
#include "vm_interface.hpp"
#include "../crypto/hash.hpp"
#include "../crypto/ed25519.hpp"
#include "../core/serialize.hpp"
#include <cstring>

namespace l1::vm {

enum class Precompile : uint8_t {
    Sha256 = 1,
    Keccak256 = 2,
    Ed25519Verify = 3,
};

core::Address precompile_address(Precompile p) {
    core::Address a = core::zero_address();
    a[19] = static_cast<uint8_t>(p);
    return a;
}

bool is_precompile(const core::Address& a) {
    for (int i = 0; i < 19; ++i) if (a[i] != 0) return false;
    return a[19] >= 1 && a[19] <= 3;
}

core::Result<VmResult> run_precompile(const core::Address& addr, const core::Bytes& input,
                                      core::Gas gas_limit) {
    VmResult vr;
    if (!is_precompile(addr)) return {core::ErrorCode::NotFound, "not precompile"};
    switch (addr[19]) {
        case 1: {
            if (gas_limit < 60) return {core::ErrorCode::VmOutOfGas, "sha256"};
            auto h = crypto::sha256(input);
            vr.output.assign(h.begin(), h.end());
            vr.gas_used = 60;
            vr.success = true;
            return vr;
        }
        case 2: {
            if (gas_limit < 30) return {core::ErrorCode::VmOutOfGas, "keccak"};
            auto h = crypto::keccak256(input);
            vr.output.assign(h.begin(), h.end());
            vr.gas_used = 30;
            vr.success = true;
            return vr;
        }
        case 3: {
            // input: pk(32) || sig(64) || msg
            if (input.size() < 96) return {core::ErrorCode::InvalidArgument, "ed25519 input"};
            if (gas_limit < 2000) return {core::ErrorCode::VmOutOfGas, "ed25519"};
            core::PublicKey pk{};
            core::Signature sig{};
            std::memcpy(pk.data(), input.data(), 32);
            std::memcpy(sig.data(), input.data() + 32, 64);
            core::Bytes msg(input.begin() + 96, input.end());
            bool ok = crypto::ed25519_verify(pk, msg, sig);
            vr.output.push_back(ok ? 1 : 0);
            vr.gas_used = 2000;
            vr.success = true;
            return vr;
        }
        default:
            return {core::ErrorCode::NotFound, "precompile"};
    }
}

}  // namespace l1::vm
