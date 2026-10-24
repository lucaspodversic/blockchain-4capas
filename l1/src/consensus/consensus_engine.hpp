// =============================================================================
// consensus_engine.hpp — Interfaz del motor de consenso (intercambiable)
// Encaja en: node loop — propone, valida, vota y finaliza.
// =============================================================================
#pragma once
#include "../core/block.hpp"
#include "../core/types.hpp"
#include "../core/error.hpp"
#include <vector>
#include <optional>
#include <functional>

namespace l1::consensus {

struct ConsensusEvent {
    enum class Kind { Proposed, Voted, Finalized, TimedOut } kind;
    core::Block block;
    core::Hash256 block_hash{};
};

class ConsensusEngine {
public:
    virtual ~ConsensusEngine() = default;

    /// Avanza el reloj de slots; puede proponer si somos proposer.
    virtual core::Result<std::vector<ConsensusEvent>> on_slot(core::Slot slot) = 0;

    /// Recibe un bloque de la red.
    virtual core::Result<std::vector<ConsensusEvent>> on_block(const core::Block& block) = 0;

    /// Recibe un voto.
    virtual core::Result<std::vector<ConsensusEvent>> on_vote(const core::Vote& vote) = 0;

    virtual core::Hash256 head() const = 0;
    virtual core::Hash256 finalized() const = 0;
    virtual core::BlockHeight height() const = 0;
};

}  // namespace l1::consensus
