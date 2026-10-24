# Layer 2 — Networks (Rust)

Rollup ZK: sequencer → executor → prover (mock) → DA → bridge → L1.

```bash
cd l2
cargo test --workspace
cargo run -p l2-sequencer -- --help
```

**Mock proof:** `SHA256(circuit_id || prev || post || tx_root || trace)`. Producción: arkworks/plonky2.
