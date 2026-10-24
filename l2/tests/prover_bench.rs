//! Benchmark del prover (mock).
//! Encaja en: mide prove/verify de N txs para CI.

use l2_common::{addr_from_u8, Batch, L2Tx};
use l2_executor::{execute_batch, L2State};
use l2_prover::{build_public_inputs, prove, verify};
use sha2::{Digest, Sha256};
use std::time::Instant;

fn sign_tx(tx: &mut L2Tx) {
    let mut h = Sha256::new();
    h.update(tx.from);
    h.update(tx.nonce.to_le_bytes());
    h.update(tx.value.to_le_bytes());
    tx.signature = h.finalize().to_vec();
}

#[test]
fn prover_throughput() {
    let n = 200usize;
    let alice = addr_from_u8(1);
    let bob = addr_from_u8(2);
    let mut state = L2State::new();
    state.credit(alice, 1_000_000);

    let mut txs = Vec::with_capacity(n);
    for i in 0..n {
        let mut tx = L2Tx {
            from: alice,
            to: bob,
            nonce: i as u64,
            value: 1,
            data: vec![],
            max_fee: 1,
            signature: vec![],
        };
        sign_tx(&mut tx);
        txs.push(tx);
    }

    let exec = execute_batch(&mut state, &txs).unwrap();
    let tx_root = Batch::compute_tx_root(&txs);

    let t0 = Instant::now();
    let proof = prove(exec.prev_root, exec.post_root, tx_root, &exec.trace).unwrap();
    let pi = build_public_inputs(exec.prev_root, exec.post_root, tx_root, &exec.trace);
    verify(&proof, &pi).unwrap();
    let dt = t0.elapsed();
    eprintln!(
        "prover_bench: txs={n} prove+verify={:?} proof_len={}",
        dt,
        proof.proof_bytes.len()
    );
    assert!(!proof.proof_bytes.is_empty());
}
