//! Genera la prueba para un lote.
//! Encaja en: relayer envía `Proof` al contrato puente L1.

use crate::circuit::{build_public_inputs, PublicInputs, CIRCUIT_ID};
use l2_common::{L2Error, Proof, Result, StateRoot};
use l2_executor::ExecutionTrace;

/// Genera mock proof determinista verificable por hash.
pub fn prove(
    prev: StateRoot,
    post: StateRoot,
    tx_root: [u8; 32],
    trace: &ExecutionTrace,
) -> Result<Proof> {
    if prev == post && trace.steps.is_empty() {
        // lote vacío: aún permitimos prueba trivial
    }
    let pi = build_public_inputs(prev, post, tx_root, trace);
    Ok(Proof {
        circuit_id: CIRCUIT_ID.into(),
        public_inputs: pi.as_hashes(),
        proof_bytes: pi.mock_proof_bytes(),
    })
}

/// Verifica el mock proof recalculando el hash.
pub fn verify(proof: &Proof, expected: &PublicInputs) -> Result<()> {
    if proof.circuit_id != CIRCUIT_ID {
        return Err(L2Error::InvalidProof("circuit_id".into()));
    }
    if proof.public_inputs != expected.as_hashes() {
        return Err(L2Error::InvalidProof("public_inputs".into()));
    }
    if proof.proof_bytes != expected.mock_proof_bytes() {
        return Err(L2Error::InvalidProof("proof_bytes mismatch".into()));
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use l2_executor::ExecutionTrace;

    #[test]
    fn prove_verify_roundtrip() {
        let prev = [1u8; 32];
        let post = [2u8; 32];
        let tx_root = [3u8; 32];
        let trace = ExecutionTrace::default();
        let p = prove(prev, post, tx_root, &trace).unwrap();
        let pi = build_public_inputs(prev, post, tx_root, &trace);
        verify(&p, &pi).unwrap();
    }
}
