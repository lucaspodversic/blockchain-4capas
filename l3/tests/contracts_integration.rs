//! Tests de contratos contra lógica nativa (nodo local opcional).
//! Encaja en: CI de L3 sin wasm32.

use contract_sdk::{addr, Ctx};
use dex_amm::{add_liquidity, swap_x_for_y};
use token_ft::{balance_of, constructor as ft_new, transfer};

#[test]
fn ft_and_dex_integration() {
    let alice = addr(1);
    let bob = addr(2);
    let ctx = Ctx::mock(alice);
    ft_new(&ctx, "USD", "USD", 6, 1_000_000);
    assert!(transfer(&ctx, bob, 10_000));
    assert_eq!(balance_of(&ctx, bob), 10_000);

    let dex = Ctx::mock(alice);
    add_liquidity(&dex, 50_000, 50_000);
    let out = swap_x_for_y(&dex, 100).expect("swap");
    assert!(out > 0);
}
