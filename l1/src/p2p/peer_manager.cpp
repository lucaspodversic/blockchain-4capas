// =============================================================================
// peer_manager.cpp — Peers, reputación y baneos
// =============================================================================
#include "peer_manager.hpp"
#include <vector>

namespace l1::p2p {

void PeerManager::add(int fd, const core::PublicKey& id, const std::string& addr) {
    std::lock_guard lock(mu_);
    if (bans_.count(id) && bans_[id]) return;
    PeerInfo p;
    p.fd = fd;
    p.id = id;
    p.addr = addr;
    p.connected_at = std::chrono::steady_clock::now();
    by_fd_[fd] = p;
}

void PeerManager::remove(int fd) {
    std::lock_guard lock(mu_);
    by_fd_.erase(fd);
}

void PeerManager::reward(int fd, int delta) {
    std::lock_guard lock(mu_);
    auto it = by_fd_.find(fd);
    if (it == by_fd_.end()) return;
    it->second.score += delta;
}

void PeerManager::penalize(int fd, int delta) {
    std::lock_guard lock(mu_);
    auto it = by_fd_.find(fd);
    if (it == by_fd_.end()) return;
    it->second.score -= delta;
    if (it->second.score <= 0) {
        bans_[it->second.id] = true;
        it->second.banned = true;
    }
}

bool PeerManager::is_banned(const core::PublicKey& id) const {
    std::lock_guard lock(mu_);
    auto it = bans_.find(id);
    return it != bans_.end() && it->second;
}

std::vector<PeerInfo> PeerManager::peers() const {
    std::lock_guard lock(mu_);
    std::vector<PeerInfo> out;
    for (const auto& [_, p] : by_fd_) out.push_back(p);
    return out;
}

size_t PeerManager::size() const {
    std::lock_guard lock(mu_);
    return by_fd_.size();
}

}  // namespace l1::p2p
