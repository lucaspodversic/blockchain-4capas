// =============================================================================
// grpc_service.cpp — Servicio para L2 / indexadores (API C++ equivalente a gRPC)
// =============================================================================
#include "grpc_service.hpp"

namespace l1::rpc {

GrpcService::GrpcService(state::StateDB* db, storage::BlockStore* blocks)
    : db_(db), blocks_(blocks) {}

core::Result<core::BlockHeight> GrpcService::GetHeight() {
    auto tip = blocks_->canonical_tip();
    if (!tip || !tip.value()) return 0ULL;
    auto b = blocks_->get_by_hash(*tip.value());
    if (!b || !b.value()) return 0ULL;
    return b.value()->header.height;
}

core::Result<core::Amount> GrpcService::GetBalance(const core::Address& a) {
    auto acc = db_->get_account(a);
    return acc ? acc->balance : 0ULL;
}

core::Result<bool> GrpcService::SubmitL2Batch(const L2BatchSubmission& batch) {
    bool ok = batch.proof_hash != core::zero_hash() &&
              batch.new_state_root != core::zero_hash();
    return ok;
}

}  // namespace l1::rpc
