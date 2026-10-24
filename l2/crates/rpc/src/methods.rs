//! Métodos del L2 (`sendTx`, `getBalance`, `getBatch`).
//! Encaja en: server despacha JSON-RPC a estas funciones.

use hex;
use l2_common::{addr_from_u8, L2Error, L2Tx, Result};
use l2_executor::L2State;
use parking_lot::Mutex;
use serde_json::{json, Value};
use std::sync::Arc;

/// Contexto compartido del RPC.
pub struct RpcState {
    pub chain_id: u64,
    pub state: Mutex<L2State>,
    pub mempool: Mutex<Vec<L2Tx>>,
    pub batches: Mutex<Vec<Value>>,
}

impl RpcState {
    pub fn new(chain_id: u64) -> Arc<Self> {
        Arc::new(Self {
            chain_id,
            state: Mutex::new(L2State::new()),
            mempool: Mutex::new(Vec::new()),
            batches: Mutex::new(Vec::new()),
        })
    }
}

fn parse_addr(s: &str) -> Result<[u8; 20]> {
    let s = s.trim_start_matches("0x");
    let b = hex::decode(s).map_err(|e| L2Error::InvalidArgument(e.to_string()))?;
    if b.len() != 20 {
        return Err(L2Error::InvalidArgument("addr len".into()));
    }
    let mut a = [0u8; 20];
    a.copy_from_slice(&b);
    Ok(a)
}

/// Despacha un método JSON-RPC.
pub fn dispatch(st: &RpcState, method: &str, params: &Value) -> Result<Value> {
    match method {
        "l2_chainId" => Ok(json!(format!("0x{:x}", st.chain_id))),
        "l2_getBalance" => {
            let addr = params
                .get(0)
                .and_then(|v| v.as_str())
                .ok_or_else(|| L2Error::InvalidArgument("addr".into()))?;
            let a = parse_addr(addr)?;
            let bal = st.state.lock().get(&a).balance;
            Ok(json!(format!("0x{:x}", bal)))
        }
        "l2_sendTx" => {
            let tx: L2Tx = serde_json::from_value(params.get(0).cloned().unwrap_or(Value::Null))
                .map_err(|e| L2Error::InvalidArgument(e.to_string()))?;
            if tx.signature.is_empty() {
                return Err(L2Error::InvalidSignature);
            }
            let h = tx.hash();
            st.mempool.lock().push(tx);
            Ok(json!(format!("0x{}", hex::encode(h))))
        }
        "l2_getBatch" => {
            let n = params
                .get(0)
                .and_then(|v| v.as_u64())
                .ok_or_else(|| L2Error::InvalidArgument("batch".into()))?;
            let batches = st.batches.lock();
            batches
                .get(n as usize)
                .cloned()
                .ok_or_else(|| L2Error::InvalidArgument("not found".into()))
        }
        "l2_blockNumber" => Ok(json!(st.batches.lock().len())),
        _ => Err(L2Error::Rpc(format!("método desconocido: {method}"))),
    }
}

/// Helper de test: acredita saldo.
pub fn credit_for_test(st: &RpcState, b: u8, amount: u64) {
    st.state.lock().credit(addr_from_u8(b), amount);
}
