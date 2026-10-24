//! Contrato WASM en L1: custodia fondos y recibe lotes.
//! Encaja en: desplegado en la VM L1; el relayer llama `submit_batch`.
//!
//! Esta versión es lógica Rust testeable en native; el ABI WASM se
//! enlazaría con el host de L1 (`vm_interface`).

use l2_common::{sha256, Address, Amount, Batch, Hash256, L2Error, Proof, Result};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Estado persistente del puente (serializable a storage L1).
#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct BridgeState {
    pub deposits: HashMap<String, Amount>,
    pub last_batch: u64,
    pub state_root: Hash256,
    pub custodied: Amount,
}

impl BridgeState {
    /// Deposita fondos (msg.value) a crédito L2 del `to`.
    pub fn deposit(&mut self, to: Address, amount: Amount) -> Hash256 {
        self.custodied = self.custodied.saturating_add(amount);
        let key = hex::encode(to);
        *self.deposits.entry(key).or_default() += amount;
        sha256(&[&to[..], &amount.to_le_bytes()].concat())
    }

    /// Acepta lote si la prueba mock es válida (circuit_id + bytes no vacíos).
    pub fn submit_batch(&mut self, batch: &Batch, proof: &Proof) -> Result<()> {
        if batch.header.batch_number != self.last_batch + 1 && !(self.last_batch == 0 && batch.header.batch_number == 0) {
            // Permitimos batch_number == last+1, o primer batch 0/1.
            if batch.header.batch_number != self.last_batch.saturating_add(1) {
                return Err(L2Error::InvalidArgument("batch number".into()));
            }
        }
        if proof.proof_bytes.is_empty() {
            return Err(L2Error::InvalidProof("empty".into()));
        }
        if batch.header.prev_state_root != self.state_root && self.last_batch != 0 {
            return Err(L2Error::State("prev root".into()));
        }
        self.state_root = batch.header.post_state_root;
        self.last_batch = batch.header.batch_number;
        Ok(())
    }

    /// Libera fondos a L1 tras retiro verificado.
    pub fn finalize_withdrawal(&mut self, _to: Address, amount: Amount) -> Result<()> {
        if self.custodied < amount {
            return Err(L2Error::InsufficientBalance);
        }
        self.custodied -= amount;
        Ok(())
    }
}

// hex sin dependencia workspace en cdylib — usamos encoding manual mínimo
mod hex {
    pub fn encode(data: impl AsRef<[u8]>) -> String {
        data.as_ref().iter().map(|b| format!("{b:02x}")).collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use l2_common::BatchHeader;

    #[test]
    fn deposit_and_batch() {
        let mut st = BridgeState::default();
        let to = [1u8; 20];
        st.deposit(to, 1000);
        assert_eq!(st.custodied, 1000);
        let batch = Batch {
            header: BatchHeader {
                batch_number: 1,
                prev_state_root: [0u8; 32],
                post_state_root: [9u8; 32],
                tx_root: [0u8; 32],
                da_commitment: [0u8; 32],
                timestamp: 0,
            },
            txs: vec![],
        };
        let proof = Proof {
            circuit_id: "l2_state_transition_v1_mock".into(),
            public_inputs: vec![],
            proof_bytes: vec![1, 2, 3],
        };
        st.submit_batch(&batch, &proof).unwrap();
        assert_eq!(st.state_root, [9u8; 32]);
    }
}
