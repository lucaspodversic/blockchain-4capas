//! Agrega varias pruebas en una sola.
//! Encaja en: reducir coste on-chain publicando N lotes con 1 prueba.

use crate::circuit::CIRCUIT_ID;
use l2_common::{sha256, L2Error, Proof, Result};

/// Agrega mock proofs: hash de concatenación de proof_bytes + public inputs.
pub fn aggregate(proofs: &[Proof]) -> Result<Proof> {
    if proofs.is_empty() {
        return Err(L2Error::EmptyBatch);
    }
    let mut buf = Vec::new();
    buf.extend_from_slice(b"agg:");
    let mut pubs = Vec::new();
    for p in proofs {
        if p.circuit_id != CIRCUIT_ID && !p.circuit_id.starts_with(CIRCUIT_ID) {
            return Err(L2Error::InvalidProof("sub circuit".into()));
        }
        buf.extend_from_slice(&p.proof_bytes);
        for pi in &p.public_inputs {
            buf.extend_from_slice(pi);
            pubs.push(*pi);
        }
    }
    Ok(Proof {
        circuit_id: format!("{CIRCUIT_ID}_agg"),
        public_inputs: pubs,
        proof_bytes: sha256(&buf).to_vec(),
    })
}

/// Verifica agregación recalculando el mismo hash.
pub fn verify_aggregate(proofs: &[Proof], agg: &Proof) -> Result<()> {
    let expected = aggregate(proofs)?;
    if expected.proof_bytes != agg.proof_bytes || expected.public_inputs != agg.public_inputs {
        return Err(L2Error::InvalidProof("agg mismatch".into()));
    }
    Ok(())
}
