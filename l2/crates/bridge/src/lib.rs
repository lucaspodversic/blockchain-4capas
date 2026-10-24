//! Puente L1 ↔ L2: cliente L1, depósitos, retiros y relayer.
pub mod deposit_watcher;
pub mod l1_client;
pub mod relayer;
pub mod withdrawal;

pub use deposit_watcher::sync_deposits;
pub use l1_client::{L1Client, L1DepositRecord, MockL1};
pub use relayer::relay_batch;
pub use withdrawal::{withdrawal_id, WithdrawalManager};
