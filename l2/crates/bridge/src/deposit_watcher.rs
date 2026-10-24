//! Detecta depósitos en L1 y acredita en L2.
//! Encaja en: bucle del sequencer / e2e antes de aceptar txs de usuario.

use crate::l1_client::L1Client;
use l2_common::{Deposit, Result};
use l2_executor::L2State;

/// Sincroniza depósitos pendientes → crédito en estado L2.
pub fn sync_deposits(l1: &dyn L1Client, state: &mut L2State) -> Result<Vec<Deposit>> {
    let pending = l1.fetch_pending_deposits();
    let mut out = Vec::new();
    for d in pending {
        state.credit(d.to, d.amount);
        l1.mark_deposit_processed(d.tx_hash);
        out.push(Deposit {
            l1_tx_hash: d.tx_hash,
            to: d.to,
            amount: d.amount,
            log_index: 0,
        });
    }
    Ok(out)
}
