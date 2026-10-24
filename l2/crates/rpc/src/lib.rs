//! Crate RPC L2.
pub mod methods;
pub mod server;

pub use methods::{credit_for_test, dispatch, RpcState};
pub use server::handle_request;
