# Guía para desarrollar contratos (L3)

## Resumen

Los contratos se escriben en **Rust** con el crate `contract-sdk` y se compilan a:

1. **Native** — tests rápidos (`cargo test -p token-ft`).
2. **wasm32-unknown-unknown** (opcional) — despliegue en la VM L1 / L1BC host.

## Estructura de un contrato

```rust
use contract_sdk::{Ctx, Address, Amount};

pub fn constructor(ctx: &Ctx, ...) { ctx.storage.set_json(...); }
pub fn mi_metodo(ctx: &Ctx, ...) -> bool { ... }
```

- `Ctx.env` — caller, value, bloque.
- `Ctx.storage` — KV JSON.
- `Ctx.events` — logs indexables.

## Despliegue

```bash
# Testnet: ver dapps/deploy/deploy_all.ts
npx ts-node dapps/deploy/deploy_all.ts
```

Direcciones en `dapps/deploy/addresses.json`.

## Estándares incluidos

| Crate | Estándar |
|-------|----------|
| token_ft | ERC-20-like |
| token_nft | ERC-721-like |
| token_multi | ERC-1155-like |
| dex_amm | Uniswap-v2-like (x·y=k) |
| staking_pool | Staking + rewards |
| governance_dao | Propuestas / votos |
| multisig | M-of-N |
| lending | Colateral + LTV |
| oracle | Precios |

## Target WASM

```bash
rustup target add wasm32-unknown-unknown   # si el toolchain lo permite
cargo build -p token-ft --target wasm32-unknown-unknown --release
```

Si el target no está disponible en el box, los tests nativos bastan para CI.
