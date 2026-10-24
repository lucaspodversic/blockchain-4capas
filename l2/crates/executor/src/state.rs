//! Estado del L2: árbol de cuentas en memoria.
//! Encaja en: executor aplica txs y calcula state root.

use l2_common::{sha256, Address, Amount, Hash256, StateRoot};
use serde::{Deserialize, Serialize};
use std::collections::BTreeMap;

/// Cuenta L2 mínima.
#[derive(Debug, Clone, Default, Serialize, Deserialize, PartialEq, Eq)]
pub struct Account {
    pub balance: Amount,
    pub nonce: u64,
}

/// Estado completo indexado por dirección (BTreeMap ⇒ raíz determinista).
#[derive(Debug, Clone, Default)]
pub struct L2State {
    accounts: BTreeMap<Address, Account>,
}

impl L2State {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn get(&self, addr: &Address) -> Account {
        self.accounts.get(addr).cloned().unwrap_or_default()
    }

    pub fn set(&mut self, addr: Address, acc: Account) {
        if acc.balance == 0 && acc.nonce == 0 {
            self.accounts.remove(&addr);
        } else {
            self.accounts.insert(addr, acc);
        }
    }

    pub fn credit(&mut self, addr: Address, amount: Amount) {
        let mut a = self.get(&addr);
        a.balance = a.balance.saturating_add(amount);
        self.set(addr, a);
    }

    /// Raíz = SHA256 de concatenación ordenada (addr||balance||nonce).
    pub fn state_root(&self) -> StateRoot {
        let mut buf = Vec::new();
        for (addr, acc) in &self.accounts {
            buf.extend_from_slice(addr);
            buf.extend_from_slice(&acc.balance.to_le_bytes());
            buf.extend_from_slice(&acc.nonce.to_le_bytes());
        }
        sha256(&buf)
    }

    pub fn accounts(&self) -> &BTreeMap<Address, Account> {
        &self.accounts
    }
}

/// Hash auxiliar exportado.
pub fn hash_pair(a: Hash256, b: Hash256) -> Hash256 {
    let mut v = Vec::with_capacity(64);
    v.extend_from_slice(&a);
    v.extend_from_slice(&b);
    sha256(&v)
}
