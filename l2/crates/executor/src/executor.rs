//! Ejecuta lotes y calcula el nuevo state root.
//! Encaja en: sequencer / e2e llaman `execute_batch`.

use crate::state::L2State;
use crate::trace::{step_from, ExecutionTrace};
use l2_common::{L2Error, L2Tx, Result, StateRoot};

/// Resultado de ejecutar un lote.
#[derive(Debug)]
pub struct ExecResult {
    pub prev_root: StateRoot,
    pub post_root: StateRoot,
    pub trace: ExecutionTrace,
}

/// Aplica una lista de txs sobre el estado (mutando).
pub fn apply_tx(state: &mut L2State, tx: &L2Tx) -> Result<()> {
    let mut from = state.get(&tx.from);
    if from.nonce != tx.nonce {
        return Err(L2Error::InvalidNonce);
    }
    let cost = tx.value.saturating_add(tx.max_fee);
    if from.balance < cost {
        return Err(L2Error::InsufficientBalance);
    }
    let fb = from.balance;
    from.balance -= cost;
    from.nonce += 1;
    state.set(tx.from, from);

    let mut to = state.get(&tx.to);
    let tb = to.balance;
    to.balance = to.balance.saturating_add(tx.value);
    state.set(tx.to, to);

    // max_fee se “quema” (simplificado; en prod va al sequencer).
    let _ = (fb, tb);
    Ok(())
}

/// Ejecuta todas las txs y construye traza.
pub fn execute_batch(state: &mut L2State, txs: &[L2Tx]) -> Result<ExecResult> {
    let prev_root = state.state_root();
    let mut trace = ExecutionTrace::default();
    for tx in txs {
        let from_b = state.get(&tx.from);
        let to_b = state.get(&tx.to);
        apply_tx(state, tx)?;
        let from_a = state.get(&tx.from);
        let to_a = state.get(&tx.to);
        trace.push(step_from(
            tx,
            from_b.balance,
            from_a.balance,
            to_b.balance,
            to_a.balance,
        ));
    }
    Ok(ExecResult {
        prev_root,
        post_root: state.state_root(),
        trace,
    })
}
