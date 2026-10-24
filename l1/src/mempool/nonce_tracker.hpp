// =============================================================================
// nonce_tracker.hpp — Tracker de nonces
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <map>
#include <mutex>

namespace l1::mempool {

class NonceTracker {
public:
    void set_account_nonce(const core::Address& addr, core::Nonce n);
    core::Nonce account_nonce(const core::Address& addr) const;
    bool is_acceptable(const core::Address& addr, core::Nonce nonce,
                       core::Nonce max_gap = 16) const;
private:
    mutable std::mutex mu_;
    std::map<core::Address, core::Nonce> account_nonce_;
};

}  // namespace l1::mempool
