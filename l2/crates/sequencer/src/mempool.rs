//! Mempool del sequencer: recibe y valida txs de usuarios.
//! Encaja en: ordering + batch_builder consumen este pool.

use l2_common::{L2Error, L2Tx, Result};
use parking_lot::Mutex;
use std::collections::{HashMap, VecDeque};

/// Mempool en memoria con índice por hash y cola FIFO.
pub struct Mempool {
    queue: Mutex<VecDeque<L2Tx>>,
    seen: Mutex<HashMap<[u8; 32], ()>>,
    max: usize,
}

impl Mempool {
    pub fn new(max: usize) -> Self {
        Self {
            queue: Mutex::new(VecDeque::new()),
            seen: Mutex::new(HashMap::new()),
            max,
        }
    }

    /// Valida estructura mínima y encola.
    pub fn add(&self, tx: L2Tx) -> Result<[u8; 32]> {
        // Firma no vacía (en prod: verificar Ed25519/secp).
        if tx.signature.is_empty() {
            return Err(L2Error::InvalidSignature);
        }
        let h = tx.hash();
        let mut seen = self.seen.lock();
        if seen.contains_key(&h) {
            return Err(L2Error::InvalidArgument("tx duplicada".into()));
        }
        let mut q = self.queue.lock();
        if q.len() >= self.max {
            return Err(L2Error::Internal("mempool llena".into()));
        }
        seen.insert(h, ());
        q.push_back(tx);
        Ok(h)
    }

    pub fn len(&self) -> usize {
        self.queue.lock().len()
    }

    /// Drena hasta `n` txs (FIFO).
    pub fn drain(&self, n: usize) -> Vec<L2Tx> {
        let mut q = self.queue.lock();
        let mut out = Vec::with_capacity(n.min(q.len()));
        for _ in 0..n {
            if let Some(tx) = q.pop_front() {
                out.push(tx);
            } else {
                break;
            }
        }
        out
    }
}
