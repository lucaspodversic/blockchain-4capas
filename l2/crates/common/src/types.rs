//! Tipos compartidos: Tx, Batch, StateRoot, Proof.
//! Encaja en: sequencer, executor, prover, bridge y RPC.

use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};

/// Hash de 32 bytes (SHA-256 en este L2 de desarrollo).
pub type Hash256 = [u8; 32];
/// Dirección de 20 bytes (compatible con L1).
pub type Address = [u8; 20];
/// Monto en unidades mínimas.
pub type Amount = u64;
/// Raíz de estado.
pub type StateRoot = Hash256;

/// Transacción L2 (transfer / call simplificado).
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct L2Tx {
    pub from: Address,
    pub to: Address,
    pub nonce: u64,
    pub value: Amount,
    pub data: Vec<u8>,
    pub max_fee: Amount,
    /// Firma Ed25519 o placeholder (64 bytes). En tests usamos hash(from||nonce||value).
    pub signature: Vec<u8>,
}

impl L2Tx {
    /// Hash canónico de la tx (sin firma) para firmar / indexar.
    pub fn signing_hash(&self) -> Hash256 {
        let mut h = Sha256::new();
        h.update(self.from);
        h.update(self.to);
        h.update(self.nonce.to_le_bytes());
        h.update(self.value.to_le_bytes());
        h.update(&(self.data.len() as u32).to_le_bytes());
        h.update(&self.data);
        h.update(self.max_fee.to_le_bytes());
        h.finalize().into()
    }

    /// Identificador de la tx (incluye firma).
    pub fn hash(&self) -> Hash256 {
        let mut h = Sha256::new();
        h.update(self.signing_hash());
        h.update(&self.signature);
        h.finalize().into()
    }
}

/// Cabecera de un lote publicado a L1.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct BatchHeader {
    pub batch_number: u64,
    pub prev_state_root: StateRoot,
    pub post_state_root: StateRoot,
    pub tx_root: Hash256,
    pub da_commitment: Hash256,
    pub timestamp: u64,
}

/// Lote completo: cabecera + txs.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Batch {
    pub header: BatchHeader,
    pub txs: Vec<L2Tx>,
}

impl Batch {
    /// Raíz Merkle simplificada (hash encadenado) de las txs.
    pub fn compute_tx_root(txs: &[L2Tx]) -> Hash256 {
        let mut acc = [0u8; 32];
        for tx in txs {
            let mut h = Sha256::new();
            h.update(acc);
            h.update(tx.hash());
            acc = h.finalize().into();
        }
        acc
    }
}

/// Prueba de validez.
///
/// **Producción:** SNARK/STARK (arkworks / plonky2).
/// **Esta fase:** “mock proof” determinista = `SHA256(prev||post||tx_root||circuit_id)`.
/// El verificador acepta si recalcula el mismo hash (documentado en `prover`).
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Proof {
    pub circuit_id: String,
    pub public_inputs: Vec<Hash256>,
    pub proof_bytes: Vec<u8>,
}

/// Evento de depósito L1 → L2.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Deposit {
    pub l1_tx_hash: Hash256,
    pub to: Address,
    pub amount: Amount,
    pub log_index: u32,
}

/// Solicitud de retiro L2 → L1.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Withdrawal {
    pub from: Address,
    pub to_l1: Address,
    pub amount: Amount,
    pub nonce: u64,
    /// Prueba de inclusión en el estado L2 (mock: hash del retiro).
    pub inclusion_proof: Vec<u8>,
}

/// Helpers hex.
pub fn hash_hex(h: &Hash256) -> String {
    hex::encode(h)
}

pub fn addr_from_u8(b: u8) -> Address {
    let mut a = [0u8; 20];
    a[19] = b;
    a
}

/// Hash genérico de bytes.
pub fn sha256(data: &[u8]) -> Hash256 {
    Sha256::digest(data).into()
}
