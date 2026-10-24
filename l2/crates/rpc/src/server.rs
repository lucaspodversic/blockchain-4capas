//! JSON-RPC del L2 (compatible con el SDK).
//! Encaja en: L3 sdk-ts apunta a este servidor.

use crate::methods::{dispatch, RpcState};
use l2_common::L2Error;
use serde_json::{json, Value};
use std::sync::Arc;

/// Maneja un body JSON-RPC 2.0 (sin HTTP; el e2e / binario puede envolverlo).
pub fn handle_request(st: &Arc<RpcState>, body: &str) -> String {
    let req: Value = match serde_json::from_str(body) {
        Ok(v) => v,
        Err(e) => {
            return json!({"jsonrpc":"2.0","id":null,"error":{"code":-32700,"message":e.to_string()}})
                .to_string();
        }
    };
    let id = req.get("id").cloned().unwrap_or(Value::Null);
    let method = req
        .get("method")
        .and_then(|m| m.as_str())
        .unwrap_or("");
    let params = req.get("params").cloned().unwrap_or(json!([]));
    match dispatch(st, method, &params) {
        Ok(result) => json!({"jsonrpc":"2.0","id":id,"result":result}).to_string(),
        Err(e) => {
            let code = match e {
                L2Error::InvalidArgument(_) => -32602,
                L2Error::Rpc(_) => -32601,
                _ => -32000,
            };
            json!({"jsonrpc":"2.0","id":id,"error":{"code":code,"message":e.to_string()}}).to_string()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::methods::credit_for_test;

    #[test]
    fn chain_id_and_balance() {
        let st = RpcState::new(42069);
        credit_for_test(&st, 1, 100);
        let resp: Value = serde_json::from_str(&handle_request(
            &st,
            r#"{"jsonrpc":"2.0","id":1,"method":"l2_chainId","params":[]}"#,
        ))
        .unwrap();
        assert_eq!(resp["result"], "0xa455");
    }
}
