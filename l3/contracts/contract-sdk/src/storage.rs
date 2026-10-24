//! Abstracción de almacenamiento clave-valor del contrato.
//! Encaja en: StateDB L1 / memoria en tests.

use serde::{de::DeserializeOwned, Serialize};
use std::collections::HashMap;
use std::sync::{Arc, Mutex};

/// Backend de storage compartible entre Ctx (Arc).
#[derive(Clone, Default)]
pub struct Storage {
    map: Arc<Mutex<HashMap<Vec<u8>, Vec<u8>>>>,
}

impl Storage {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn set_raw(&self, key: &[u8], val: &[u8]) {
        self.map.lock().unwrap().insert(key.to_vec(), val.to_vec());
    }

    pub fn get_raw(&self, key: &[u8]) -> Option<Vec<u8>> {
        self.map.lock().unwrap().get(key).cloned()
    }

    pub fn set_json<T: Serialize>(&self, key: &str, val: &T) {
        let b = serde_json::to_vec(val).expect("serialize");
        self.set_raw(key.as_bytes(), &b);
    }

    pub fn get_json<T: DeserializeOwned>(&self, key: &str) -> Option<T> {
        let b = self.get_raw(key.as_bytes())?;
        serde_json::from_slice(&b).ok()
    }
}
