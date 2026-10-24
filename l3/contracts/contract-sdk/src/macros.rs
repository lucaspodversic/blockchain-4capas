//! Macros de ayuda `#[contract]` / `#[entry]` (simplificadas).
//! Encaja en: marcadores documentales; en prod generarían ABI/WASM exports.

/// Marca un módulo como contrato (documentación + futuro codegen).
#[macro_export]
macro_rules! contract {
    ($name:ident) => {
        pub const CONTRACT_NAME: &str = stringify!($name);
    };
}

/// Marca una función como entrypoint público.
#[macro_export]
macro_rules! entry {
    ($fn:item) => {
        $fn
    };
}
