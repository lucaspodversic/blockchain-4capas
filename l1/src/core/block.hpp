// =============================================================================
// block.hpp — Bloque = cabecera + transacciones + agregados de firma BLS
// Encaja en: consenso, sync, almacenamiento y RPC getBlock.
// =============================================================================
#pragma once

#include "block_header.hpp"
#include "transaction.hpp"
#include <vector>

namespace l1::core {

/// Voto de un validador sobre un bloque (para finalidad).
struct Vote {
    Hash256 block_hash{zero_hash()};
    Slot slot{0};
    Address validator{zero_address()};
    Signature signature{};
};

struct Block {
    BlockHeader header;
    std::vector<Transaction> transactions;
    /// Firmas/votos de comité (pueden agregarse off-band vía BLS).
    std::vector<Vote> votes;
    Signature proposer_signature{};
    bool has_proposer_sig{false};

    Bytes encode() const;
    static Result<Block> decode(const Bytes& raw);

    /// Hash del bloque = hash de la cabecera (estilo Ethereum).
    Hash256 hash() const;

    /// Recalcula tx_root a partir de las transacciones.
    void compute_tx_root();

    Result<void> validate_integrity() const;
};

}  // namespace l1::core
