//! API pública del SDK de contratos L3.
//! Encaja en: todos los contratos estándar dependen de este crate.

pub mod env;
pub mod events;
pub mod macros;
pub mod storage;

pub use env::{Address, Amount, Env};
pub use events::{Event, EventLog};
pub use storage::Storage;

/// Contexto completo pasado a cada entrypoint en tests.
pub struct Ctx {
    pub env: Env,
    pub storage: Storage,
    pub events: EventLog,
}

impl Ctx {
    pub fn mock(caller: Address) -> Self {
        Self {
            env: Env::mock(caller),
            storage: Storage::new(),
            events: EventLog::default(),
        }
    }
}

pub fn addr(b: u8) -> Address {
    let mut a = [0u8; 20];
    a[19] = b;
    a
}
