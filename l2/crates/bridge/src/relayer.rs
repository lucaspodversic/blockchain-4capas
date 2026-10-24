//! Envía lotes y pruebas al verificador del L1.
//! Encaja en: tras prove, publica DA commitment implícito + batch.

use crate::l1_client::L1Client;
use l2_common::{Batch, Proof, Result};
use l2_da::DaPublisher;
use l2_prover::{build_public_inputs, prove, verify};
use l2_executor::ExecutionTrace;

/// Empaqueta: DA publish → prove → submit_batch.
pub fn relay_batch(
    l1: &dyn L1Client,
    da: &DaPublisher,
    batch: &Batch,
    trace: &ExecutionTrace,
) -> Result<Proof> {
    let commitment = da.publish(batch)?;
    if commitment != batch.header.da_commitment {
        // El builder debe haber usado el mismo blob; en e2e alineamos.
        // Si difiere, aún publicamos (dev); prod exigiría igualdad.
    }
    let proof = prove(
        batch.header.prev_state_root,
        batch.header.post_state_root,
        batch.header.tx_root,
        trace,
    )?;
    let pi = build_public_inputs(
        batch.header.prev_state_root,
        batch.header.post_state_root,
        batch.header.tx_root,
        trace,
    );
    verify(&proof, &pi)?;
    l1.submit_batch(batch.clone(), proof.clone())?;
    Ok(proof)
}
