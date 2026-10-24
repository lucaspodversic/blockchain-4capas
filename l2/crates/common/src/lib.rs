//! Crate común del L2: tipos, config y errores.
//! Encaja en: base de sequencer, executor, prover, da, bridge y rpc.

pub mod config;
pub mod error;
pub mod types;

pub use config::L2Config;
pub use error::{L2Error, Result};
pub use types::*;
