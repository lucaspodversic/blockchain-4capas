//! Exchange descentralizado (AMM x·y=k).
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
mod pool;
pub use pool::{get_amount_out, mint_lp, Reserves};

#[derive(Default, Serialize, Deserialize)]
struct State {
    reserve_x: u128,
    reserve_y: u128,
    total_lp: u128,
    lp_balances: std::collections::HashMap<String, u128>,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn add_liquidity(ctx: &Ctx, dx: Amount, dy: Amount) -> u128 {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let r = Reserves { x: s.reserve_x, y: s.reserve_y };
    let lp = mint_lp(dx as u128, dy as u128, s.total_lp, r);
    s.reserve_x += dx as u128;
    s.reserve_y += dy as u128;
    s.total_lp += lp;
    *s.lp_balances.entry(hx(&ctx.env.caller)).or_default() += lp;
    ctx.storage.set_json("s", &s);
    lp
}

pub fn swap_x_for_y(ctx: &Ctx, dx: Amount) -> Option<Amount> {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let out = get_amount_out(dx as u128, s.reserve_x, s.reserve_y)? as Amount;
    s.reserve_x += dx as u128;
    s.reserve_y -= out as u128;
    ctx.storage.set_json("s", &s);
    ctx.events.emit("Swap", vec![hx(&ctx.env.caller)], serde_json::json!({"dx":dx,"dy":out}));
    Some(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn swap() {
        let ctx = Ctx::mock(addr(1));
        add_liquidity(&ctx, 100_000, 100_000);
        let out = swap_x_for_y(&ctx, 1000).unwrap();
        assert!(out > 900);
    }
}
