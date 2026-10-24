// =============================================================================
// l2_verifier.cpp — Verificación de batches L2
// =============================================================================
#include "l2_verifier.hpp"
#include "logger.hpp"

namespace l1::node {

core::Result<void> L2Verifier::verify_and_apply(const rpc::L2BatchSubmission& batch) {
    if (batch.proof_hash == core::zero_hash()) {
        return {core::ErrorCode::InvalidArgument, "empty proof"};
    }
    if (last_root_ != core::zero_hash() && batch.prev_state_root != last_root_) {
        return {core::ErrorCode::InvalidArgument, "prev_root mismatch"};
    }
    // Placeholder: en producción verificar SNARK/STARK.
    last_root_ = batch.new_state_root;
    ++batch_count_;
    log_info("L2 batch #" + std::to_string(batch_count_) + " accepted");
    return core::Result<void>::Ok();
}

}  // namespace l1::node
