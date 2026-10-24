//! Procesa retiros L2 → L1 con prueba de inclusión.
//! Encaja en: usuario quema fondos en L2; tras delay, L1 libera.

use l2_common::{sha256, Address, Amount, L2Error, Result, Withdrawal};
use l2_executor::L2State;
use parking_lot::Mutex;
use std::collections::HashMap;

/// Gestor de retiros pendientes.
#[derive(Default)]
pub struct WithdrawalManager {
    pending: Mutex<HashMap<[u8; 32], (Withdrawal, u64)>>, // id -> (w, batch_submitted)
}

impl WithdrawalManager {
    pub fn new() -> Self {
        Self::default()
    }

    /// Inicia retiro: debita L2 y registra con prueba mock.
    pub fn initiate(&self, state: &mut L2State, from: Address, to_l1: Address, amount: Amount) -> Result<Withdrawal> {
        let mut acc = state.get(&from);
        if acc.balance < amount {
            return Err(L2Error::InsufficientBalance);
        }
        acc.balance -= amount;
        let nonce = acc.nonce;
        state.set(from, acc);

        let mut w = Withdrawal {
            from,
            to_l1,
            amount,
            nonce,
            inclusion_proof: vec![],
        };
        // Mock inclusion proof = hash del retiro.
        let id = withdrawal_id(&w);
        w.inclusion_proof = id.to_vec();
        self.pending.lock().insert(id, (w.clone(), 0));
        Ok(w)
    }

    pub fn mark_batch(&self, id: [u8; 32], batch_no: u64) {
        if let Some(e) = self.pending.lock().get_mut(&id) {
            e.1 = batch_no;
        }
    }

    /// Tras `delay` lotes, se puede finalizar en L1.
    pub fn take_ready(&self, current_batch: u64, delay: u64) -> Vec<Withdrawal> {
        let mut pending = self.pending.lock();
        let mut ready = Vec::new();
        let keys: Vec<_> = pending
            .iter()
            .filter(|(_, (_, b))| *b > 0 && current_batch >= *b + delay)
            .map(|(k, _)| *k)
            .collect();
        for k in keys {
            if let Some((w, _)) = pending.remove(&k) {
                ready.push(w);
            }
        }
        ready
    }
}

pub fn withdrawal_id(w: &Withdrawal) -> [u8; 32] {
    let mut buf = Vec::new();
    buf.extend_from_slice(&w.from);
    buf.extend_from_slice(&w.to_l1);
    buf.extend_from_slice(&w.amount.to_le_bytes());
    buf.extend_from_slice(&w.nonce.to_le_bytes());
    sha256(&buf)
}
