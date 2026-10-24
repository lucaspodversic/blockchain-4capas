//! Política de orden: FIFO + prioridad por fee.
//! Encaja en: batch_builder ordena antes de sellar.

use l2_common::L2Tx;

/// Ordena txs: mayor `max_fee` primero; empate mantiene orden relativo (estable).
pub fn order_by_fee(mut txs: Vec<L2Tx>) -> Vec<L2Tx> {
    // sort_by estable en Rust desde 1.x para slice::sort_by? Actually sort_by is unstable.
    // Usamos sort_by_key con índice para estabilidad.
    let mut indexed: Vec<(usize, L2Tx)> = txs.drain(..).enumerate().collect();
    indexed.sort_by(|a, b| {
        b.1.max_fee
            .cmp(&a.1.max_fee)
            .then_with(|| a.0.cmp(&b.0))
    });
    indexed.into_iter().map(|(_, t)| t).collect()
}

#[cfg(test)]
mod tests {
    use super::*;
    use l2_common::addr_from_u8;

    fn tx(fee: u64, sig: u8) -> L2Tx {
        L2Tx {
            from: addr_from_u8(1),
            to: addr_from_u8(2),
            nonce: 0,
            value: 1,
            data: vec![],
            max_fee: fee,
            signature: vec![sig],
        }
    }

    #[test]
    fn fee_priority() {
        let ordered = order_by_fee(vec![tx(1, 1), tx(10, 2), tx(5, 3)]);
        assert_eq!(ordered[0].max_fee, 10);
        assert_eq!(ordered[1].max_fee, 5);
        assert_eq!(ordered[2].max_fee, 1);
    }
}
