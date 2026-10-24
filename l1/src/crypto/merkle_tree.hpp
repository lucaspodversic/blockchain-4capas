// =============================================================================
// merkle_tree.hpp — Árbol de Merkle (raíces y pruebas de inclusión)
// Encaja en: tx_root, receipts_root y sync.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include <vector>

namespace l1::crypto {

class MerkleTree {
public:
    /// Calcula la raíz Merkle de una lista de hojas (keccak de pares).
    /// Lista vacía → zero_hash. Un solo elemento → ese hash.
    static core::Hash256 root(std::vector<core::Hash256> leaves);

    struct Proof {
        std::vector<core::Hash256> siblings;  ///< De hoja a raíz
        std::vector<bool> is_right;           ///< true si el sibling está a la derecha
    };

    /// Construye prueba de inclusión para leaves[index].
    static Proof prove(std::vector<core::Hash256> leaves, size_t index);

    /// Verifica que leaf + proof producen expected_root.
    static bool verify(const core::Hash256& leaf, const Proof& proof,
                       const core::Hash256& expected_root);
};

}  // namespace l1::crypto
