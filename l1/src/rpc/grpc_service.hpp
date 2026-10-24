// =============================================================================
// grpc_service.hpp — API estilo gRPC (sin dependencia protobuf en esta fase)
// =============================================================================
#pragma once
#include "../state/state_db.hpp"
#include "../storage/block_store.hpp"

namespace l1::rpc {

struct L2BatchSubmission {
    core::Hash256 prev_state_root{};
    core::Hash256 new_state_root{};
    core::Hash256 proof_hash{};
    core::Bytes da_commitment;
};

class GrpcService {
public:
    GrpcService(state::StateDB* db, storage::BlockStore* blocks);
    core::Result<core::BlockHeight> GetHeight();
    core::Result<core::Amount> GetBalance(const core::Address& a);
    core::Result<bool> SubmitL2Batch(const L2BatchSubmission& batch);
private:
    state::StateDB* db_;
    storage::BlockStore* blocks_;
};

}  // namespace l1::rpc
