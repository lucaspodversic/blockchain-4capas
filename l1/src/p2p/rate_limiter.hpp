// =============================================================================
// rate_limiter.hpp — Límite de tasa contra spam/DoS
// =============================================================================
#pragma once
#include <chrono>
#include <cstdint>

namespace l1::p2p {

class RateLimiter {
public:
    RateLimiter(uint64_t max_per_window, std::chrono::milliseconds window)
        : max_(max_per_window), window_(window), count_(0) {
        reset_at_ = std::chrono::steady_clock::now() + window_;
    }

    bool allow() {
        auto now = std::chrono::steady_clock::now();
        if (now >= reset_at_) {
            count_ = 0;
            reset_at_ = now + window_;
        }
        if (count_ >= max_) return false;
        ++count_;
        return true;
    }

private:
    uint64_t max_;
    std::chrono::milliseconds window_;
    uint64_t count_;
    std::chrono::steady_clock::time_point reset_at_;
};

}  // namespace l1::p2p
