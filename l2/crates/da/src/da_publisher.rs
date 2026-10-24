//! Publica los datos comprimidos (simula calldata / blob L1).
//! Encaja en: bridge/relayer obtiene el commitment publicado.

use crate::compressor::compress_batch;
use l2_common::{sha256, Batch, Hash256, Result};
use std::sync::Mutex;

/// Almacén DA en proceso (tests / dev). En prod: posting a L1 o Celestia/etc.
#[derive(Default)]
pub struct DaPublisher {
    blobs: Mutex<Vec<(Hash256, Vec<u8>)>>,
}

impl DaPublisher {
    pub fn new() -> Self {
        Self::default()
    }

    /// Publica batch y devuelve commitment = SHA256(blob).
    pub fn publish(&self, batch: &Batch) -> Result<Hash256> {
        let blob = compress_batch(batch)?;
        let commitment = sha256(&blob);
        self.blobs.lock().unwrap().push((commitment, blob));
        Ok(commitment)
    }

    pub fn get(&self, commitment: &Hash256) -> Option<Vec<u8>> {
        self.blobs
            .lock()
            .unwrap()
            .iter()
            .find(|(c, _)| c == commitment)
            .map(|(_, b)| b.clone())
    }

    pub fn len(&self) -> usize {
        self.blobs.lock().unwrap().len()
    }
}
