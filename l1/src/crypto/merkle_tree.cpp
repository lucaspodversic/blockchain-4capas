// =============================================================================
// merkle_tree.cpp — Construcción de raíces y pruebas Merkle
// =============================================================================
#include "merkle_tree.hpp"
#include "hash.hpp"

namespace l1::crypto {

core::Hash256 MerkleTree::root(std::vector<core::Hash256> leaves) {
    if (leaves.empty()) return core::zero_hash();
    while (leaves.size() > 1) {
        if (leaves.size() % 2 == 1) leaves.push_back(leaves.back());  // duplicar última
        std::vector<core::Hash256> next;
        next.reserve(leaves.size() / 2);
        for (size_t i = 0; i < leaves.size(); i += 2) {
            next.push_back(hash_pair(leaves[i], leaves[i + 1]));
        }
        leaves.swap(next);
    }
    return leaves[0];
}

MerkleTree::Proof MerkleTree::prove(std::vector<core::Hash256> leaves, size_t index) {
    Proof p;
    if (leaves.empty() || index >= leaves.size()) return p;
    size_t idx = index;
    while (leaves.size() > 1) {
        if (leaves.size() % 2 == 1) leaves.push_back(leaves.back());
        size_t sibling = (idx % 2 == 0) ? idx + 1 : idx - 1;
        p.siblings.push_back(leaves[sibling]);
        p.is_right.push_back(idx % 2 == 0);  // sibling a la derecha si idx es par
        std::vector<core::Hash256> next;
        next.reserve(leaves.size() / 2);
        for (size_t i = 0; i < leaves.size(); i += 2) {
            next.push_back(hash_pair(leaves[i], leaves[i + 1]));
        }
        leaves.swap(next);
        idx /= 2;
    }
    return p;
}

bool MerkleTree::verify(const core::Hash256& leaf, const Proof& proof,
                        const core::Hash256& expected_root) {
    core::Hash256 h = leaf;
    for (size_t i = 0; i < proof.siblings.size(); ++i) {
        if (proof.is_right[i]) h = hash_pair(h, proof.siblings[i]);
        else h = hash_pair(proof.siblings[i], h);
    }
    return h == expected_root;
}

}  // namespace l1::crypto
