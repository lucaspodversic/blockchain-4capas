//! Cliente RPC hacia el nodo L1 (C++).
//! Encaja en: deposit_watcher y relayer hablan con L1 vía JSON-RPC.
//!
//! En tests usamos `MockL1` en memoria que simula el contrato puente.

use l2_common::{sha256, Address, Amount, Batch, Hash256, L2Error, Proof, Result};
use parking_lot::Mutex;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Registro de un depósito en el puente L1 simulado.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct L1DepositRecord {
    pub tx_hash: Hash256,
    pub to: Address,
    pub amount: Amount,
    pub processed: bool,
}

/// Estado del contrato puente mock en L1.
#[derive(Default)]
pub struct MockL1 {
    pub deposits: Mutex<Vec<L1DepositRecord>>,
    pub batches: Mutex<Vec<(Batch, Proof)>>,
    /// Fondos custodiados por dirección L1 de destino de retiros.
    pub balances: Mutex<HashMap<Address, Amount>>,
    pub finalized_roots: Mutex<Vec<Hash256>>,
}

impl MockL1 {
    pub fn new() -> Self {
        Self::default()
    }

    /// Usuario deposita en L1 (lock funds).
    pub fn deposit(&self, to: Address, amount: Amount) -> Hash256 {
        let mut h = sha256(&[&to[..], &amount.to_le_bytes()].concat());
        // uniquify with length
        let n = self.deposits.lock().len() as u64;
        h = sha256(&[&h[..], &n.to_le_bytes()].concat());
        self.deposits.lock().push(L1DepositRecord {
            tx_hash: h,
            to,
            amount,
            processed: false,
        });
        // Custodia
        *self.balances.lock().entry([0u8; 20]).or_default() += amount;
        h
    }

    /// Relayer publica lote + prueba.
    pub fn submit_batch(&self, batch: Batch, proof: Proof) -> Result<()> {
        // Verificación mock: el bridge contract llama al verifier.
        let pi = l2_prover::build_public_inputs(
            batch.header.prev_state_root,
            batch.header.post_state_root,
            batch.header.tx_root,
            &l2_executor::ExecutionTrace::default(), // el e2e pasa proof ya válida
        );
        // Aceptamos la prueba si proof_bytes no está vacío y circuit_id correcto.
        if proof.proof_bytes.is_empty() {
            return Err(L2Error::InvalidProof("empty".into()));
        }
        if !proof.circuit_id.contains("l2_state_transition") {
            return Err(L2Error::InvalidProof("circuit".into()));
        }
        let _ = pi;
        self.finalized_roots
            .lock()
            .push(batch.header.post_state_root);
        self.batches.lock().push((batch, proof));
        Ok(())
    }

    /// Ejecuta retiro liberando fondos custodiados.
    pub fn finalize_withdrawal(&self, to_l1: Address, amount: Amount) -> Result<()> {
        let mut bal = self.balances.lock();
        let bridge = bal.entry([0u8; 20]).or_default();
        if *bridge < amount {
            return Err(L2Error::InsufficientBalance);
        }
        *bridge -= amount;
        *bal.entry(to_l1).or_default() += amount;
        Ok(())
    }

    pub fn l1_balance(&self, addr: &Address) -> Amount {
        *self.balances.lock().get(addr).unwrap_or(&0)
    }
}

/// Trait de cliente L1 (RPC real o mock).
pub trait L1Client: Send + Sync {
    fn fetch_pending_deposits(&self) -> Vec<L1DepositRecord>;
    fn mark_deposit_processed(&self, tx_hash: Hash256);
    fn submit_batch(&self, batch: Batch, proof: Proof) -> Result<()>;
    fn finalize_withdrawal(&self, to_l1: Address, amount: Amount) -> Result<()>;
}

impl L1Client for MockL1 {
    fn fetch_pending_deposits(&self) -> Vec<L1DepositRecord> {
        self.deposits
            .lock()
            .iter()
            .filter(|d| !d.processed)
            .cloned()
            .collect()
    }
    fn mark_deposit_processed(&self, tx_hash: Hash256) {
        for d in self.deposits.lock().iter_mut() {
            if d.tx_hash == tx_hash {
                d.processed = true;
            }
        }
    }
    fn submit_batch(&self, batch: Batch, proof: Proof) -> Result<()> {
        MockL1::submit_batch(self, batch, proof)
    }
    fn finalize_withdrawal(&self, to_l1: Address, amount: Amount) -> Result<()> {
        MockL1::finalize_withdrawal(self, to_l1, amount)
    }
}
