//! Préstamos con colateral.
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct State {
    collateral: HashMap<String, Amount>,
    debt: HashMap<String, Amount>,
    ltv_bps: u64, // max loan-to-value
    price: Amount, // precio colateral en unidad de deuda
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn constructor(ctx: &Ctx, ltv_bps: u64, price: Amount) {
    ctx.storage.set_json("s", &State { ltv_bps, price, ..Default::default() });
}

pub fn deposit_collateral(ctx: &Ctx, amount: Amount) {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    *s.collateral.entry(hx(&ctx.env.caller)).or_default() += amount;
    ctx.storage.set_json("s", &s);
}

pub fn borrow(ctx: &Ctx, amount: Amount) -> bool {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let k = hx(&ctx.env.caller);
    let col = *s.collateral.get(&k).unwrap_or(&0);
    let max = (col as u128 * s.price as u128 * s.ltv_bps as u128 / 10_000) as Amount;
    let debt = *s.debt.get(&k).unwrap_or(&0);
    if debt + amount > max { return false; }
    *s.debt.entry(k).or_default() += amount;
    ctx.storage.set_json("s", &s);
    true
}

pub fn repay(ctx: &Ctx, amount: Amount) {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let k = hx(&ctx.env.caller);
    let d = s.debt.get(&k).copied().unwrap_or(0);
    s.debt.insert(k, d.saturating_sub(amount));
    ctx.storage.set_json("s", &s);
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn lend() {
        let ctx = Ctx::mock(addr(1));
        constructor(&ctx, 5000, 2); // 50% LTV, price 2
        deposit_collateral(&ctx, 100);
        assert!(borrow(&ctx, 100)); // max = 100*2*0.5 = 100
        assert!(!borrow(&ctx, 1));
    }
}
