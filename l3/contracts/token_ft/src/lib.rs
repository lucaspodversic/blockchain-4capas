//! Token fungible (tipo ERC-20).
//! Encaja en: dapps de swap/staking usan este estándar.

use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct Meta {
    name: String,
    symbol: String,
    decimals: u8,
    total_supply: Amount,
    balances: HashMap<String, Amount>,
    allowances: HashMap<String, Amount>,
}

fn key(a: &Address) -> String {
    hex::encode(a)
}

fn allow_key(owner: &Address, spender: &Address) -> String {
    format!("{}:{}", key(owner), key(spender))
}

/// Despliega con supply inicial al caller.
pub fn constructor(ctx: &Ctx, name: &str, symbol: &str, decimals: u8, supply: Amount) {
    let mut m = Meta {
        name: name.into(),
        symbol: symbol.into(),
        decimals,
        total_supply: supply,
        ..Default::default()
    };
    m.balances.insert(key(&ctx.env.caller), supply);
    ctx.storage.set_json("meta", &m);
    ctx.events.emit(
        "Transfer",
        vec![hex::encode([0u8; 20]), hex::encode(ctx.env.caller)],
        serde_json::json!({"value": supply}),
    );
}

pub fn balance_of(ctx: &Ctx, who: Address) -> Amount {
    let m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    *m.balances.get(&key(&who)).unwrap_or(&0)
}

pub fn transfer(ctx: &Ctx, to: Address, amount: Amount) -> bool {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    let from = key(&ctx.env.caller);
    let bal = *m.balances.get(&from).unwrap_or(&0);
    if bal < amount {
        return false;
    }
    m.balances.insert(from.clone(), bal - amount);
    let tb = *m.balances.get(&key(&to)).unwrap_or(&0);
    m.balances.insert(key(&to), tb + amount);
    ctx.storage.set_json("meta", &m);
    ctx.events.emit(
        "Transfer",
        vec![hex::encode(ctx.env.caller), hex::encode(to)],
        serde_json::json!({"value": amount}),
    );
    true
}

pub fn approve(ctx: &Ctx, spender: Address, amount: Amount) -> bool {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    m.allowances
        .insert(allow_key(&ctx.env.caller, &spender), amount);
    ctx.storage.set_json("meta", &m);
    true
}

pub fn transfer_from(ctx: &Ctx, from: Address, to: Address, amount: Amount) -> bool {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    let ak = allow_key(&from, &ctx.env.caller);
    let allow = *m.allowances.get(&ak).unwrap_or(&0);
    let fb = *m.balances.get(&key(&from)).unwrap_or(&0);
    if allow < amount || fb < amount {
        return false;
    }
    m.allowances.insert(ak, allow - amount);
    m.balances.insert(key(&from), fb - amount);
    let tb = *m.balances.get(&key(&to)).unwrap_or(&0);
    m.balances.insert(key(&to), tb + amount);
    ctx.storage.set_json("meta", &m);
    true
}

mod hex {
    pub fn encode(d: impl AsRef<[u8]>) -> String {
        d.as_ref().iter().map(|b| format!("{b:02x}")).collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn transfer_works() {
        let alice = addr(1);
        let bob = addr(2);
        let ctx = Ctx::mock(alice);
        constructor(&ctx, "Coin", "COIN", 18, 1000);
        assert_eq!(balance_of(&ctx, alice), 1000);
        assert!(transfer(&ctx, bob, 100));
        assert_eq!(balance_of(&ctx, bob), 100);
        assert_eq!(balance_of(&ctx, alice), 900);
    }
}
