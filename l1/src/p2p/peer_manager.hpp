// =============================================================================
// peer_manager.hpp — Gestión de peers, reputación y baneos
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>

namespace l1::p2p {

struct PeerInfo {
    int fd{-1};
    core::PublicKey id{};
    std::string addr;
    int score{100};
    bool banned{false};
    std::chrono::steady_clock::time_point connected_at{};
};

class PeerManager {
public:
    void add(int fd, const core::PublicKey& id, const std::string& addr);
    void remove(int fd);
    void reward(int fd, int delta = 1);
    void penalize(int fd, int delta = 10);
    bool is_banned(const core::PublicKey& id) const;
    std::vector<PeerInfo> peers() const;
    size_t size() const;

private:
    mutable std::mutex mu_;
    std::map<int, PeerInfo> by_fd_;
    std::map<core::PublicKey, bool> bans_;
};

}  // namespace l1::p2p
