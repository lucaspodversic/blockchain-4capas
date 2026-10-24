//! Arma los lotes de transacciones.
//! Encaja en: el bucle del sequencer sella Batch con cabecera y DA commitment.

use crate::ordering::order_by_fee;
use l2_common::{sha256, Batch, BatchHeader, L2Tx, StateRoot};
use std::time::{SystemTime, UNIX_EPOCH};

/// Construye un `Batch` a partir de txs y raíces prev/post.
pub fn build_batch(
    batch_number: u64,
    prev: StateRoot,
    post: StateRoot,
    txs: Vec<L2Tx>,
    da_blob: &[u8],
) -> Batch {
    let txs = order_by_fee(txs);
    let tx_root = Batch::compute_tx_root(&txs);
    let da_commitment = sha256(da_blob);
    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_secs())
        .unwrap_or(0);
    Batch {
        header: BatchHeader {
            batch_number,
            prev_state_root: prev,
            post_state_root: post,
            tx_root,
            da_commitment,
            timestamp,
        },
        txs,
    }
}
