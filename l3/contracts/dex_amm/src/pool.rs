//! Lógica de pools de liquidez (x·y=k).
//! Encaja en: dex_amm::swap llama a estas funciones puras.

/// Reserva de un par.
#[derive(Debug, Clone, Copy, Default)]
pub struct Reserves {
    pub x: u128,
    pub y: u128,
}

/// Calcula output con fee 0.3% (997/1000).
pub fn get_amount_out(amount_in: u128, reserve_in: u128, reserve_out: u128) -> Option<u128> {
    if amount_in == 0 || reserve_in == 0 || reserve_out == 0 {
        return None;
    }
    let amount_in_with_fee = amount_in.checked_mul(997)?;
    let num = amount_in_with_fee.checked_mul(reserve_out)?;
    let den = reserve_in.checked_mul(1000)?.checked_add(amount_in_with_fee)?;
    Some(num / den)
}

/// Liquidez mint: lp = sqrt(dx*dy) simplificado (mínimo).
pub fn mint_lp(dx: u128, dy: u128, total_lp: u128, r: Reserves) -> u128 {
    if total_lp == 0 {
        // primer depósito
        return integer_sqrt(dx.saturating_mul(dy));
    }
    let lx = dx.saturating_mul(total_lp) / r.x.max(1);
    let ly = dy.saturating_mul(total_lp) / r.y.max(1);
    lx.min(ly)
}

fn integer_sqrt(n: u128) -> u128 {
    if n == 0 { return 0; }
    let mut x = n;
    let mut y = (x + 1) / 2;
    while y < x {
        x = y;
        y = (x + n / x) / 2;
    }
    x
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn constant_product() {
        let out = get_amount_out(1000, 100_000, 100_000).unwrap();
        assert!(out > 0 && out < 1000);
    }
}
