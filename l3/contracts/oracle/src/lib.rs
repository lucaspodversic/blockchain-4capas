//! Oráculo de precios.
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct State {
    owner: String,
    prices: HashMap<String, Amount>,
    updated_at: HashMap<String, u64>,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn constructor(ctx: &Ctx) {
    let s = State { owner: hx(&ctx.env.caller), ..Default::default() };
    ctx.storage.set_json("s", &s);
}

pub fn set_price(ctx: &Ctx, symbol: &str, price: Amount) -> bool {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    if hx(&ctx.env.caller) != s.owner { return false; }
    s.prices.insert(symbol.into(), price);
    s.updated_at.insert(symbol.into(), ctx.env.timestamp);
    ctx.storage.set_json("s", &s);
    true
}

pub fn get_price(ctx: &Ctx, symbol: &str) -> Option<Amount> {
    let s: State = ctx.storage.get_json("s").unwrap_or_default();
    s.prices.get(symbol).copied()
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn oracle_price() {
        let ctx = Ctx::mock(addr(1));
        constructor(&ctx);
        assert!(set_price(&ctx, "BTC", 60_000));
        assert_eq!(get_price(&ctx, "BTC"), Some(60_000));
    }
}
