// =============================================================================
// rpc_auth.hpp — Autenticación y límites de la API
// =============================================================================
#pragma once
#include <string>
#include <mutex>
#include <chrono>
#include <map>
#include <memory>

namespace l1::rpc {

/// Control de token Bearer y rate-limit por IP.
/// No copiable (mutex); movable vía unique_ptr interno.
class RpcAuth {
public:
    RpcAuth() : mu_(std::make_unique<std::mutex>()) {}
    RpcAuth(RpcAuth&&) noexcept = default;
    RpcAuth& operator=(RpcAuth&&) noexcept = default;
    RpcAuth(const RpcAuth&) = delete;
    RpcAuth& operator=(const RpcAuth&) = delete;

    void set_api_token(std::string token) { token_ = std::move(token); }
    bool require_auth() const { return !token_.empty(); }

    bool check_bearer(const std::string& header) const {
        if (token_.empty()) return true;
        const std::string prefix = "Bearer ";
        if (header.rfind(prefix, 0) != 0) return false;
        return header.substr(prefix.size()) == token_;
    }

    /// Rate limit simple por IP (ventana deslizante de 1 minuto).
    bool allow_ip(const std::string& ip, uint64_t max_per_min = 600) {
        std::lock_guard lock(*mu_);
        auto now = std::chrono::steady_clock::now();
        auto& e = hits_[ip];
        if (now - e.window_start > std::chrono::minutes(1)) {
            e.window_start = now;
            e.count = 0;
        }
        if (e.count >= max_per_min) return false;
        ++e.count;
        return true;
    }

private:
    std::string token_;
    struct Hit { std::chrono::steady_clock::time_point window_start{}; uint64_t count{0}; };
    std::unique_ptr<std::mutex> mu_;
    std::map<std::string, Hit> hits_;
};

}  // namespace l1::rpc
