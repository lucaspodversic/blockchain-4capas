//! Propuestas y votación on-chain.
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Clone, Serialize, Deserialize)]
struct Proposal {
    id: u64,
    description: String,
    yes: Amount,
    no: Amount,
    executed: bool,
    voters: HashMap<String, bool>,
}

#[derive(Default, Serialize, Deserialize)]
struct State {
    next_id: u64,
    proposals: HashMap<String, Proposal>,
    voting_power: HashMap<String, Amount>,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn set_power(ctx: &Ctx, who: Address, power: Amount) {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    s.voting_power.insert(hx(&who), power);
    ctx.storage.set_json("s", &s);
}

pub fn propose(ctx: &Ctx, description: &str) -> u64 {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let id = s.next_id;
    s.next_id += 1;
    s.proposals.insert(id.to_string(), Proposal {
        id, description: description.into(), yes: 0, no: 0, executed: false, voters: HashMap::new(),
    });
    ctx.storage.set_json("s", &s);
    id
}

pub fn vote(ctx: &Ctx, id: u64, support: bool) -> bool {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let power = *s.voting_power.get(&hx(&ctx.env.caller)).unwrap_or(&0);
    let p = match s.proposals.get_mut(&id.to_string()) {
        Some(p) => p,
        None => return false,
    };
    if p.voters.contains_key(&hx(&ctx.env.caller)) { return false; }
    p.voters.insert(hx(&ctx.env.caller), true);
    if support { p.yes += power; } else { p.no += power; }
    ctx.storage.set_json("s", &s);
    true
}

pub fn execute(ctx: &Ctx, id: u64) -> bool {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let p = match s.proposals.get_mut(&id.to_string()) {
        Some(p) => p,
        None => return false,
    };
    if p.executed || p.yes <= p.no { return false; }
    p.executed = true;
    ctx.storage.set_json("s", &s);
    true
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn dao_flow() {
        let a = addr(1);
        let ctx = Ctx::mock(a);
        set_power(&ctx, a, 10);
        let id = propose(&ctx, "Upgrade");
        assert!(vote(&ctx, id, true));
        assert!(execute(&ctx, id));
    }
}
