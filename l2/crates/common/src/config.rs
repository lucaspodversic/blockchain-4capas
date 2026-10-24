//! Configuración del L2.
//! Encaja en: sequencer, RPC y bridge leen `L2Config`.

use serde::{Deserialize, Serialize};

/// Parámetros operativos del rollup.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct L2Config {
    /// Identificador de cadena L2.
    pub chain_id: u64,
    /// URL JSON-RPC del nodo L1 (C++).
    pub l1_rpc_url: String,
    /// Dirección del contrato puente en L1 (hex).
    pub bridge_address: String,
    /// Máximo de txs por lote.
    pub max_txs_per_batch: usize,
    /// Intervalo de sellado de lotes (ms).
    pub batch_interval_ms: u64,
    /// Puerto del JSON-RPC L2.
    pub rpc_port: u16,
    /// Tiempo de challenge para retiros (slots L1 simulados).
    pub withdrawal_delay_batches: u64,
}

impl Default for L2Config {
    fn default() -> Self {
        Self {
            chain_id: 42069,
            l1_rpc_url: "http://127.0.0.1:8545".into(),
            bridge_address: "0x00000000000000000000000000000000000000BB".into(),
            max_txs_per_batch: 1000,
            batch_interval_ms: 2000,
            rpc_port: 8546,
            withdrawal_delay_batches: 1,
        }
    }
}

impl L2Config {
    /// Carga desde JSON; si falla, usa defaults.
    pub fn from_json(s: &str) -> Self {
        serde_json::from_str(s).unwrap_or_default()
    }
}
