//! Wallet multifirma.
use contract_sdk::{addr, Address, Amount, Ctx, Env};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct State {
    owners: Vec<String>,
    threshold: u32,
    nonce: u64,
    approvals: HashMap<String, Vec<String>>,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn constructor(ctx: &Ctx, owners: Vec<Address>, threshold: u32) {
    let s = State { owners: owners.iter().map(hx).collect(), threshold, ..Default::default() };
    ctx.storage.set_json("s", &s);
}

pub fn submit(ctx: &Ctx, to: Address, value: Amount) -> String {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let id = format!("{}:{}:{}", s.nonce, hx(&to), value);
    s.nonce += 1;
    s.approvals.insert(id.clone(), vec![hx(&ctx.env.caller)]);
    ctx.storage.set_json("s", &s);
    id
}

pub fn approve(ctx: &Ctx, id: &str) -> bool {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let caller = hx(&ctx.env.caller);
    if !s.owners.contains(&caller) { return false; }
    let list = s.approvals.entry(id.into()).or_default();
    if !list.contains(&caller) { list.push(caller); }
    ctx.storage.set_json("s", &s);
    true
}

pub fn can_execute(ctx: &Ctx, id: &str) -> bool {
    let s: State = ctx.storage.get_json("s").unwrap_or_default();
    s.approvals.get(id).map(|a| a.len() as u32 >= s.threshold).unwrap_or(false)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn multisig_threshold() {
        let a = addr(1);
        let b = addr(2);
        let ctx = Ctx::mock(a);
        constructor(&ctx, vec![a, b], 2);
        let id = submit(&ctx, addr(9), 50);
        assert!(!can_execute(&ctx, &id));
        let ctx_b = Ctx {
            env: Env { caller: b, ..ctx.env.clone() },
            storage: ctx.storage.clone(),
            events: ctx.events.clone(),
        };
        assert!(approve(&ctx_b, &id));
        assert!(can_execute(&ctx, &id));
    }
}
