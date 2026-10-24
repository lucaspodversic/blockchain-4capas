//! Traza de ejecución para el prover.
//! Encaja en: cada paso (pre/post cuenta) alimenta el circuito mock.

use l2_common::{Address, Amount, Hash256, L2Tx};
use serde::{Deserialize, Serialize};

/// Un paso de la traza (antes/después de aplicar una tx).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TraceStep {
    pub tx_hash: Hash256,
    pub from: Address,
    pub to: Address,
    pub value: Amount,
    pub from_balance_before: Amount,
    pub from_balance_after: Amount,
    pub to_balance_before: Amount,
    pub to_balance_after: Amount,
}

/// Traza completa de un lote.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ExecutionTrace {
    pub steps: Vec<TraceStep>,
}

impl ExecutionTrace {
    pub fn push(&mut self, step: TraceStep) {
        self.steps.push(step);
    }

    /// Compromiso de la traza (para public inputs del mock proof).
    pub fn commitment(&self) -> Hash256 {
        use sha2::{Digest, Sha256};
        let mut h = Sha256::new();
        for s in &self.steps {
            h.update(s.tx_hash);
            h.update(s.value.to_le_bytes());
            h.update(s.from_balance_before.to_le_bytes());
            h.update(s.from_balance_after.to_le_bytes());
        }
        h.finalize().into()
    }
}

/// Helper: crea paso a partir de balances y tx.
pub fn step_from(tx: &L2Tx, fb: Amount, fa: Amount, tb: Amount, ta: Amount) -> TraceStep {
    TraceStep {
        tx_hash: tx.hash(),
        from: tx.from,
        to: tx.to,
        value: tx.value,
        from_balance_before: fb,
        from_balance_after: fa,
        to_balance_before: tb,
        to_balance_after: ta,
    }
}
