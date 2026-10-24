// =============================================================================
// chain_params.hpp — Parámetros de la cadena (tiempo de bloque, gas, recompensas)
// Encaja en: consenso, mempool, génesis y config del nodo.
// =============================================================================
#pragma once

#include "types.hpp"
#include <string>

namespace l1::core {

struct ChainParams {
    ChainId chain_id{1337};
    std::string network_name{"l1-devnet"};

    /// Duración objetivo de slot/bloque en milisegundos.
    uint64_t slot_ms{1000};

    /// Slots por época (rotación de validadores / recompensas).
    uint64_t slots_per_epoch{32};

    /// Límite de gas por bloque.
    Gas block_gas_limit{30'000'000};

    /// Gas base de una transferencia simple.
    Gas tx_base_gas{21000};

    /// Recompensa al proposer por bloque (unidad mínima).
    Amount block_reward{2'000'000'000ULL};  // 2e9

    /// Stake mínimo para ser validador.
    Amount min_validator_stake{32'000'000'000ULL};

    /// Base fee inicial (EIP-1559-like).
    Amount initial_base_fee{1ULL};  // 1 unidad/gas (devnet)

    /// Elasticidad del mercado de fees (fracción del gas objetivo = límite/2).
    uint64_t fee_elasticity{2};

    /// Máximo de validadores activos.
    uint32_t max_validators{100};

    /// Umbral BFT: 2/3 del stake (expresado en bps, 6667 ≈ 66.67%).
    uint32_t finality_threshold_bps{6667};

    static ChainParams devnet() {
        ChainParams p;
        p.network_name = "l1-devnet";
        p.chain_id = 1337;
        p.slot_ms = 500;
        p.slots_per_epoch = 8;
        p.block_gas_limit = 10'000'000;
        return p;
    }

    static ChainParams mainnet_defaults() {
        ChainParams p;
        p.network_name = "l1-mainnet";
        p.chain_id = 1;
        p.slot_ms = 1000;
        p.slots_per_epoch = 32;
        return p;
    }
};

}  // namespace l1::core
