//! Errores unificados del L2.
//! Encaja en: todos los crates propagan `L2Error` vía `Result`.

use thiserror::Error;

/// Códigos de error tipados del rollup.
#[derive(Debug, Error, Clone, PartialEq, Eq)]
pub enum L2Error {
    #[error("argumento inválido: {0}")]
    InvalidArgument(String),
    #[error("saldo insuficiente")]
    InsufficientBalance,
    #[error("nonce inválido")]
    InvalidNonce,
    #[error("firma inválida")]
    InvalidSignature,
    #[error("lote vacío")]
    EmptyBatch,
    #[error("prueba inválida: {0}")]
    InvalidProof(String),
    #[error("depósito no encontrado")]
    DepositNotFound,
    #[error("retiro pendiente / challenge")]
    WithdrawalPending,
    #[error("RPC: {0}")]
    Rpc(String),
    #[error("estado inconsistente: {0}")]
    State(String),
    #[error("interno: {0}")]
    Internal(String),
}

/// Alias de resultado del L2.
pub type Result<T> = std::result::Result<T, L2Error>;
