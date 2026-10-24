//! Arranque del sequencer L2.
//! Encaja en: proceso que escucha txs, sella lotes, prueba y relay a L1.
//!
//! Uso: `l2-sequencer --help`

use clap::Parser;
use l2_common::L2Config;
use tracing::info;

mod batch_builder;
mod mempool;
mod ordering;

#[derive(Parser, Debug)]
#[command(name = "l2-sequencer", about = "Sequencer del rollup L2")]
struct Args {
    /// Ruta a config JSON (opcional).
    #[arg(long)]
    config: Option<String>,
}

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();
    let args = Args::parse();
    let cfg = if let Some(path) = args.config {
        let s = std::fs::read_to_string(path).unwrap_or_default();
        L2Config::from_json(&s)
    } else {
        L2Config::default()
    };
    info!(
        chain_id = cfg.chain_id,
        rpc = cfg.rpc_port,
        "sequencer L2 listo (modo librería; e2e en tests)"
    );
    // El bucle de producción engancha mempool → executor → prover → bridge.
    // La lógica completa se ejercita en `tests/e2e_rollup.rs`.
}
