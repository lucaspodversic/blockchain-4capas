// =============================================================================
// genesis.hpp — Bloque y estado génesis (alloc + validadores iniciales)
// Encaja en: arranque del nodo y tools/genesis_gen.
// =============================================================================
#pragma once

#include "block.hpp"
#include "account.hpp"
#include "chain_params.hpp"
#include <map>
#include <vector>

namespace l1::core {

struct GenesisValidator {
    Address address{zero_address()};
    PublicKey pubkey{};
    Amount stake{0};
};

struct GenesisConfig {
    ChainParams params{ChainParams::devnet()};
    uint64_t timestamp{0};
    /// Allocaciones iniciales address -> balance.
    std::map<Address, Amount> alloc;
    std::vector<GenesisValidator> validators;
    Bytes extra_data;

    /// Construye el bloque génesis (altura 0, parent = cero).
    Block build_block() const;

    /// Serializa / deserializa desde JSON (nlohmann).
    std::string to_json() const;
    static Result<GenesisConfig> from_json(const std::string& json);
};

}  // namespace l1::core
