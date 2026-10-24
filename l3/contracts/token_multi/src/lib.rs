//! Multi-token (tipo ERC-1155).
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct Meta {
    balances: HashMap<String, Amount>, // "id:addr" -> amount
    uris: HashMap<String, String>,
}

fn k(id: u64, a: &Address) -> String {
    format!("{}:{}", id, a.iter().map(|b| format!("{b:02x}")).collect::<String>())
}

pub fn mint(ctx: &Ctx, to: Address, id: u64, amount: Amount, uri: &str) {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    *m.balances.entry(k(id, &to)).or_default() += amount;
    m.uris.insert(id.to_string(), uri.into());
    ctx.storage.set_json("meta", &m);
}

pub fn balance_of(ctx: &Ctx, who: Address, id: u64) -> Amount {
    let m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    *m.balances.get(&k(id, &who)).unwrap_or(&0)
}

pub fn safe_transfer(ctx: &Ctx, to: Address, id: u64, amount: Amount) -> bool {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    let fk = k(id, &ctx.env.caller);
    let bal = *m.balances.get(&fk).unwrap_or(&0);
    if bal < amount { return false; }
    m.balances.insert(fk, bal - amount);
    *m.balances.entry(k(id, &to)).or_default() += amount;
    ctx.storage.set_json("meta", &m);
    true
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn multi() {
        let a = addr(1); let b = addr(2);
        let ctx = Ctx::mock(a);
        mint(&ctx, a, 7, 50, "ipfs://m");
        assert_eq!(balance_of(&ctx, a, 7), 50);
        assert!(safe_transfer(&ctx, b, 7, 20));
        assert_eq!(balance_of(&ctx, b, 7), 20);
    }
}
