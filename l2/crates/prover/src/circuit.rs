//! Circuito que codifica las reglas de transición de estado.
//!
//! **Producción:** circuito R1CS/Plonkish (arkworks / plonky2) que verifica
//! cada paso de la traza (balances, nonces, firma).
//! **Esta fase:** interfaz correcta + `MockCircuit` que compromete public inputs.

use l2_common::{sha256, Hash256, StateRoot};
use l2_executor::ExecutionTrace;

/// Identificador del circuito de transición.
pub const CIRCUIT_ID: &str = "l2_state_transition_v1_mock";

/// Entradas públicas del circuito (visibles on-chain).
#[derive(Debug, Clone)]
pub struct PublicInputs {
    pub prev_state_root: StateRoot,
    pub post_state_root: StateRoot,
    pub tx_root: Hash256,
    pub trace_commitment: Hash256,
}

impl PublicInputs {
    /// Serializa a lista de hashes para `Proof.public_inputs`.
    pub fn as_hashes(&self) -> Vec<Hash256> {
        vec![
            self.prev_state_root,
            self.post_state_root,
            self.tx_root,
            self.trace_commitment,
        ]
    }

    /// Compromiso único usado como cuerpo del mock proof.
    pub fn mock_proof_bytes(&self) -> Vec<u8> {
        let mut buf = Vec::new();
        buf.extend_from_slice(CIRCUIT_ID.as_bytes());
        for h in self.as_hashes() {
            buf.extend_from_slice(&h);
        }
        sha256(&buf).to_vec()
    }
}

/// Construye public inputs a partir de raíces y traza.
pub fn build_public_inputs(
    prev: StateRoot,
    post: StateRoot,
    tx_root: Hash256,
    trace: &ExecutionTrace,
) -> PublicInputs {
    PublicInputs {
        prev_state_root: prev,
        post_state_root: post,
        tx_root,
        trace_commitment: trace.commitment(),
    }
}
