// =============================================================================
// nonce_tracker.cpp — Control de nonces por cuenta
// =============================================================================
#include "nonce_tracker.hpp"

namespace l1::mempool {

void NonceTracker::set_account_nonce(const core::Address& addr, core::Nonce n) {
    std::lock_guard lock(mu_);
    account_nonce_[addr] = n;
}

core::Nonce NonceTracker::account_nonce(const core::Address& addr) const {
    std::lock_guard lock(mu_);
    auto it = account_nonce_.find(addr);
    return it == account_nonce_.end() ? 0 : it->second;
}

bool NonceTracker::is_acceptable(const core::Address& addr, core::Nonce nonce,
                                 core::Nonce max_gap) const {
    auto cur = account_nonce(addr);
    return nonce >= cur && nonce <= cur + max_gap;
}

}  // namespace l1::mempool
