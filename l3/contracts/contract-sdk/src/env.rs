//! Acceso al entorno de ejecución (caller, bloque, valor).
//! Encaja en: contratos leen `Env` inyectado por el host / tests.

use serde::{Deserialize, Serialize};

pub type Address = [u8; 20];
pub type Amount = u64;

/// Contexto de la llamada actual.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Env {
    pub caller: Address,
    pub contract: Address,
    pub value: Amount,
    pub block_height: u64,
    pub timestamp: u64,
}

impl Env {
    pub fn mock(caller: Address) -> Self {
        Self {
            caller,
            contract: [0xC0; 20],
            value: 0,
            block_height: 1,
            timestamp: 1_700_000_000,
        }
    }
}
