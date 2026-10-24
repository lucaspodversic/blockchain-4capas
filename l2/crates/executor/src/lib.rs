//! Crate executor: estado L2 + aplicación de lotes + traza.
//! Encaja en: sequencer y prover consumen `execute_batch` / `ExecutionTrace`.

pub mod executor;
pub mod state;
pub mod trace;

pub use executor::{apply_tx, execute_batch, ExecResult};
pub use state::{Account, L2State};
pub use trace::{ExecutionTrace, TraceStep};
