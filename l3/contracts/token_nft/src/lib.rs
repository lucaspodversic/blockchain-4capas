//! NFT (tipo ERC-721).
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct Meta {
    name: String,
    symbol: String,
    owner_of: HashMap<String, String>,
    balances: HashMap<String, u64>,
    token_uri: HashMap<String, String>,
    next_id: u64,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn constructor(ctx: &Ctx, name: &str, symbol: &str) {
    let m = Meta { name: name.into(), symbol: symbol.into(), ..Default::default() };
    ctx.storage.set_json("meta", &m);
}

pub fn mint(ctx: &Ctx, to: Address, uri: &str) -> u64 {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    let id = m.next_id;
    m.next_id += 1;
    m.owner_of.insert(id.to_string(), hx(&to));
    *m.balances.entry(hx(&to)).or_default() += 1;
    m.token_uri.insert(id.to_string(), uri.into());
    ctx.storage.set_json("meta", &m);
    ctx.events.emit("Transfer", vec!["0".into(), hx(&to), id.to_string()], serde_json::json!({}));
    id
}

pub fn owner_of(ctx: &Ctx, id: u64) -> Option<String> {
    let m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    m.owner_of.get(&id.to_string()).cloned()
}

pub fn transfer(ctx: &Ctx, to: Address, id: u64) -> bool {
    let mut m: Meta = ctx.storage.get_json("meta").unwrap_or_default();
    let from = hx(&ctx.env.caller);
    match m.owner_of.get(&id.to_string()) {
        Some(o) if o == &from => {}
        _ => return false,
    }
    m.owner_of.insert(id.to_string(), hx(&to));
    *m.balances.entry(from).or_default() -= 1;
    *m.balances.entry(hx(&to)).or_default() += 1;
    ctx.storage.set_json("meta", &m);
    true
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn mint_transfer() {
        let a = addr(1); let b = addr(2);
        let ctx = Ctx::mock(a);
        constructor(&ctx, "Art", "ART");
        let id = mint(&ctx, a, "ipfs://x");
        assert_eq!(owner_of(&ctx, id).unwrap(), hx(&a));
        assert!(transfer(&ctx, b, id));
        assert_eq!(owner_of(&ctx, id).unwrap(), hx(&b));
    }
}
