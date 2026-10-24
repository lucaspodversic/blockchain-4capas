// =============================================================================
// discovery_kademlia.cpp — Tabla Kademlia simplificada
// =============================================================================
#include "discovery_kademlia.hpp"
#include <algorithm>

namespace l1::p2p {

KademliaTable::KademliaTable(core::PublicKey self_id) : self_(std::move(self_id)) {}

int KademliaTable::xor_distance_msb(const core::PublicKey& a, const core::PublicKey& b) {
    for (size_t i = 0; i < 32; ++i) {
        uint8_t x = a[i] ^ b[i];
        if (x == 0) continue;
        for (int bit = 7; bit >= 0; --bit) {
            if (x & (1 << bit)) return static_cast<int>(i * 8 + (7 - bit));
        }
    }
    return 256;
}

void KademliaTable::upsert(const NodeRecord& n) {
    if (n.id == self_) return;
    nodes_[n.id] = n;
}

std::vector<NodeRecord> KademliaTable::closest(const core::PublicKey& target, size_t k) const {
    std::vector<std::pair<int, NodeRecord>> scored;
    for (const auto& [_, n] : nodes_) {
        scored.push_back({xor_distance_msb(n.id, target), n});
    }
    std::sort(scored.begin(), scored.end(),
              [](auto& a, auto& b) { return a.first < b.first; });
    std::vector<NodeRecord> out;
    for (size_t i = 0; i < scored.size() && i < k; ++i) out.push_back(scored[i].second);
    return out;
}

std::vector<NodeRecord> KademliaTable::all() const {
    std::vector<NodeRecord> out;
    for (const auto& [_, n] : nodes_) out.push_back(n);
    return out;
}

}  // namespace l1::p2p
