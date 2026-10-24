//! Test de extremo a extremo: depósito → lote → retiro.
//! Encaja en: valida el flujo completo del rollup con mock proof + MockL1.

use l2_bridge::{relay_batch, sync_deposits, withdrawal_id, MockL1, WithdrawalManager};
use l2_common::{addr_from_u8, Batch, BatchHeader, L2Tx};
use l2_da::DaPublisher;
use l2_executor::{execute_batch, L2State};
use l2_prover::{build_public_inputs, verify};
use rollup_bridge::BridgeState;
use sha2::{Digest, Sha256};

/// Firma de prueba: hash(from||nonce||value) como bytes.
fn sign_tx(tx: &mut L2Tx) {
    let mut h = Sha256::new();
    h.update(tx.from);
    h.update(tx.nonce.to_le_bytes());
    h.update(tx.value.to_le_bytes());
    tx.signature = h.finalize().to_vec();
}

#[test]
fn deposit_batch_withdraw_e2e() {
    // --- L1 mock + contrato puente lógico ---
    let l1 = MockL1::new();
    let mut bridge = BridgeState::default();
    let da = DaPublisher::new();
    let mut state = L2State::new();
    let withdrawals = WithdrawalManager::new();

    let alice = addr_from_u8(0xA1);
    let bob = addr_from_u8(0xB0);
    let alice_l1 = addr_from_u8(0x11);

    // 1) Depósito L1 → crédito L2
    let dep_amount = 10_000u64;
    let _dep_hash = l1.deposit(alice, dep_amount);
    bridge.deposit(alice, dep_amount);
    let deps = sync_deposits(&l1, &mut state).unwrap();
    assert_eq!(deps.len(), 1);
    assert_eq!(state.get(&alice).balance, dep_amount);

    // 2) Tx L2: Alice paga a Bob
    let mut tx = L2Tx {
        from: alice,
        to: bob,
        nonce: 0,
        value: 1_000,
        data: vec![],
        max_fee: 10,
        signature: vec![],
    };
    sign_tx(&mut tx);

    let prev = state.state_root();
    let exec = execute_batch(&mut state, &[tx.clone()]).unwrap();
    assert_eq!(exec.prev_root, prev);
    assert_eq!(state.get(&bob).balance, 1_000);
    assert_eq!(state.get(&alice).balance, dep_amount - 1_000 - 10);

    // 3) Construir lote + DA commitment alineado
    let txs = vec![tx];
    let tx_root = Batch::compute_tx_root(&txs);
    // Pre-comprimimos para commitment
    let mut batch = Batch {
        header: BatchHeader {
            batch_number: 1,
            prev_state_root: exec.prev_root,
            post_state_root: exec.post_root,
            tx_root,
            da_commitment: [0u8; 32],
            timestamp: 1,
        },
        txs,
    };
    let blob = l2_da::compress_batch(&batch).unwrap();
    batch.header.da_commitment = l2_common::sha256(&blob);

    // 4) Probar + relay a L1
    let proof = relay_batch(&l1, &da, &batch, &exec.trace).unwrap();
    let pi = build_public_inputs(exec.prev_root, exec.post_root, tx_root, &exec.trace);
    verify(&proof, &pi).unwrap();
    bridge.submit_batch(&batch, &proof).unwrap();
    assert_eq!(bridge.state_root, exec.post_root);

    // 5) Retiro: Bob saca fondos a L1
    let w = withdrawals
        .initiate(&mut state, bob, alice_l1, 500)
        .unwrap();
    assert_eq!(state.get(&bob).balance, 500);
    let wid = withdrawal_id(&w);
    withdrawals.mark_batch(wid, batch.header.batch_number);
    let ready = withdrawals.take_ready(batch.header.batch_number + 1, 1);
    assert_eq!(ready.len(), 1);
    l1.finalize_withdrawal(alice_l1, 500).unwrap();
    bridge.finalize_withdrawal(alice_l1, 500).unwrap();
    assert_eq!(l1.l1_balance(&alice_l1), 500);
}
