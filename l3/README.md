# Layer 3 — Dapps

- `contracts/` — Rust→WASM SDK + estándares
- `sdk-ts/` — cliente JSON-RPC TypeScript
- `indexer/` — indexador Go (SQLite en tests, Postgres en prod)
- `dapps/` — deploy + lógicas de ejemplo
- `docs/CONTRACT_GUIDE.md`

```bash
cd contracts && cargo test --workspace
cd ../sdk-ts && npm test
cd ../indexer && go test ./...
```
