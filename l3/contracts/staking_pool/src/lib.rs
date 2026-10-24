//! Staking y delegación.
#[allow(unused_imports)]
use contract_sdk::{addr, Address, Amount, Ctx};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Default, Serialize, Deserialize)]
struct State {
    total_staked: Amount,
    stakes: HashMap<String, Amount>,
    reward_rate_bps: u64, // por época (simplificado)
    rewards: HashMap<String, Amount>,
}

fn hx(a: &Address) -> String { a.iter().map(|b| format!("{b:02x}")).collect() }

pub fn constructor(ctx: &Ctx, reward_rate_bps: u64) {
    let s = State { reward_rate_bps, ..Default::default() };
    ctx.storage.set_json("s", &s);
}

pub fn stake(ctx: &Ctx, amount: Amount) {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    *s.stakes.entry(hx(&ctx.env.caller)).or_default() += amount;
    s.total_staked += amount;
    ctx.storage.set_json("s", &s);
}

pub fn accrue(ctx: &Ctx) {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    for (k, st) in s.stakes.clone() {
        let r = (st as u128 * s.reward_rate_bps as u128 / 10_000) as Amount;
        *s.rewards.entry(k).or_default() += r;
    }
    ctx.storage.set_json("s", &s);
}

pub fn claim(ctx: &Ctx) -> Amount {
    let mut s: State = ctx.storage.get_json("s").unwrap_or_default();
    let k = hx(&ctx.env.caller);
    let r = s.rewards.remove(&k).unwrap_or(0);
    ctx.storage.set_json("s", &s);
    r
}

pub fn staked_of(ctx: &Ctx, who: Address) -> Amount {
    let s: State = ctx.storage.get_json("s").unwrap_or_default();
    *s.stakes.get(&hx(&who)).unwrap_or(&0)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn stake_reward() {
        let a = addr(1);
        let ctx = Ctx::mock(a);
        constructor(&ctx, 1000); // 10%
        stake(&ctx, 1000);
        accrue(&ctx);
        assert_eq!(claim(&ctx), 100);
    }
}
