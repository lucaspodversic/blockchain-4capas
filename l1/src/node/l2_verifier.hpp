// =============================================================================
// l2_verifier.hpp — Verifica pruebas/batches enviados por L2
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"
#include "../rpc/grpc_service.hpp"
#include <map>

namespace l1::node {

class L2Verifier {
public:
    core::Result<void> verify_and_apply(const rpc::L2BatchSubmission& batch);
    core::Hash256 last_state_root() const { return last_root_; }

private:
    core::Hash256 last_root_{core::zero_hash()};
    uint64_t batch_count_{0};
};

}  // namespace l1::node
