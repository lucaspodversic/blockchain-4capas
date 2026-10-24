//! Crate prover: circuito mock + prove/verify + agregación.
//!
//! Producción sustituye `prove`/`verify` por arkworks o plonky2 manteniendo
//! la misma firma de `Proof` y `PublicInputs`.

pub mod aggregator;
pub mod circuit;
pub mod prover;

pub use aggregator::{aggregate, verify_aggregate};
pub use circuit::{build_public_inputs, PublicInputs, CIRCUIT_ID};
pub use prover::{prove, verify};
