// =============================================================================
// transaction.hpp — Transacción firmada (transferencia o llamada a contrato)
// Encaja en: mempool, ejecución de bloques, P2P gossip y RPC sendTx.
// =============================================================================
#pragma once

#include "types.hpp"
#include "serialize.hpp"
#include "error.hpp"

namespace l1::core {

/// Tipo de transacción.
enum class TxType : uint8_t {
    Transfer = 0,   ///< Envío de valor entre cuentas
    Deploy = 1,     ///< Despliegue de contrato (data = bytecode)
    Call = 2,       ///< Llamada a contrato (to = contrato, data = input)
};

struct Transaction {
    TxType type{TxType::Transfer};
    Address from{zero_address()};
    Address to{zero_address()};      ///< Vacío/cero en Deploy
    Nonce nonce{0};
    Amount value{0};
    Gas gas_limit{21000};
    Amount max_fee_per_gas{0};       ///< Fee máximo (EIP-1559-like)
    Amount max_priority_fee{0};      ///< Tip al proposer
    Bytes data;                      ///< Input / bytecode
    ChainId chain_id{1337};
    Signature signature{};           ///< Firma Ed25519 sobre signing_hash()
    bool has_signature{false};

    /// Serializa campos firmables (sin la firma).
    Bytes encode_unsigned() const;

    /// Serializa tx completa (con firma).
    Bytes encode() const;

    static Result<Transaction> decode(const Bytes& raw);

    /// Hash canónico de la tx firmada (id de la tx).
    Hash256 hash() const;

    /// Hash sobre el que se firma (encode_unsigned + keccak).
    Hash256 signing_hash() const;

    /// Validación estructural (no comprueba firma ni saldo).
    Result<void> validate_structure() const;
};

}  // namespace l1::core
