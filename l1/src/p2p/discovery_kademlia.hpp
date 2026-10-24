// =============================================================================
// discovery_kademlia.hpp — Descubrimiento Kademlia DHT (tabla de routing)
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <vector>
#include <string>
#include <array>
#include <map>

namespace l1::p2p {

struct NodeRecord {
    core::PublicKey id{};
    std::string host;
    uint16_t port{0};
};

class KademliaTable {
public:
    explicit KademliaTable(core::PublicKey self_id);
    void upsert(const NodeRecord& n);
    std::vector<NodeRecord> closest(const core::PublicKey& target, size_t k = 16) const;
    std::vector<NodeRecord> all() const;
    size_t size() const { return nodes_.size(); }

private:
    static int xor_distance_msb(const core::PublicKey& a, const core::PublicKey& b);
    core::PublicKey self_;
    std::map<core::PublicKey, NodeRecord> nodes_;
};

}  // namespace l1::p2p
