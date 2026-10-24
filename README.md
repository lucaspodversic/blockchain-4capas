# Blockchain 4 Capas / 4-Layer Blockchain

Monorepo de una pila blockchain completa en **4 capas**, cada una en su propio directorio. Listo para GitHub.

```
L4 UI  ──HTTP/WS──►  L3 Dapps  ──JSON-RPC──►  L2 Networks  ──batches+proofs──►  L1 Blockchain
```

| Capa | Directorio | Rol | Lenguaje | Archivos plan |
|------|------------|-----|----------|---------------|
| **L1** | `l1/` | Consenso, estado, VM, P2P, RPC | C++20 | **94/94** |
| **L2** | `l2/` | Sequencer, executor, prover, DA, bridge, RPC | Rust | **30/30** |
| **L3** | `l3/` | Contratos WASM, SDK TS, indexador Go, dapps | Rust / TS / Go | **46/46** |
| **L4** | `l4/` | Web Next.js, móvil Flutter, extensión MV3 | TS / Dart | **56/56** |

Total plan: **226** archivos de código listados en `PLAN.md` (todos `[x]`).

---

## Cómo se hablan las capas

1. **L4 → L3**: HTTP/WebSocket hacia el SDK TypeScript, indexador Go y frontends.
2. **L3 → L2**: el SDK envía txs al sequencer (JSON-RPC).
3. **L2 → L1**: el rollup publica batches + pruebas al puente en L1 (`rollup_bridge`).
4. **L3 → L1**: lecturas y txs L1 vía JSON-RPC del nodo (`l1d`).

---

## LAYER 1 — Blockchain (C++20)

```bash
cd l1
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

Binarios: `l1d`, `l1_cli`, `wallet_cli`, `genesis_gen`.

**Fallbacks:** `memory_store` (sin RocksDB); BLS/VRF simplificados; TCP POSIX (sin Asio); VM bytecode **L1BC** (wasmtime-ready); gRPC/QUIC stubs.

---

## LAYER 2 — Networks (Rust)

Workspace: `common`, `sequencer`, `executor`, `prover`, `da`, `bridge`, `rpc`, `contracts/rollup_bridge`, `tests`.

```bash
cd l2
cargo test --workspace
```

El **prover ZK** expone una interfaz de circuito correcta con *mock proof* determinista (hash/state-root matching). Documentado: producción usaría arkworks/plonky2. El e2e **deposit → batch → withdrawal** debe pasar (`tests/e2e_rollup`).

---

## LAYER 3 — Dapps

### Contratos (`l3/contracts`)

```bash
cd l3/contracts && cargo test --workspace
# Opcional WASM:
# rustup target add wasm32-unknown-unknown
# cargo build -p token-ft --target wasm32-unknown-unknown --release
```

SDK `contract-sdk` + FT/NFT/multi, DEX AMM, staking, DAO, multisig, lending, oracle. Guía: `l3/docs/CONTRACT_GUIDE.md`.

### SDK TypeScript (`l3/sdk-ts`)

```bash
cd l3/sdk-ts && npm test
```

### Indexador Go (`l3/indexer`)

```bash
cd l3/indexer && go test ./...
```

**Tests:** SQLite in-memory. **Producción:** Postgres (`internal/db/schema.sql`).

### Dapps

`l3/dapps/deploy/` + lógicas swap/staking/gov/nft_market.

---

## LAYER 4 — User Interface

```bash
cd l4/web && npm install && npm test          # format helpers
cd l4/extension && npm test                   # vault XOR roundtrip
# Móvil (si Flutter instalado):
# cd l4/mobile && flutter pub get && flutter test
```

- `web/` — Next.js/React/Tailwind (wallet, swap, stake, gov, NFT, explorer).
- `mobile/` — Flutter (fuentes completas; Flutter opcional en CI).
- `extension/` — Manifest V3 (`window.l1`).
- `shared/` — design tokens + locales ES/EN.
- `e2e/` — Playwright + Dart specs.

---

## Raíz GitHub-ready

- `LICENSE` — MIT
- `PLAN.md` — checklist completo `[x]`
- `CONTRIBUTING.md` — guía de contribución
- `.gitignore` — `target/`, `build/`, `node_modules/`, `.dart_tool/`, etc.

---

## Zip de entrega

```bash
# Generado en el box (sin artefactos de build):
# /workspace/blockchain_4capas_completo.zip
```
