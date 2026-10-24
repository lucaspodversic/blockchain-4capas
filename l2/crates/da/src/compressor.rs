//! Comprime los datos del lote (JSON + RLE trivial de ceros).
//! Encaja en: da_publisher envía el blob comprimido / commitment a L1.

use l2_common::{Batch, L2Error, Result};
use serde_json;

/// Serializa el lote a bytes JSON.
pub fn encode_batch(batch: &Batch) -> Result<Vec<u8>> {
    serde_json::to_vec(batch).map_err(|e| L2Error::Internal(e.to_string()))
}

/// Compresión simple: si hay muchos ceros, marca runs (demo).
/// Formato: 0x00 | raw JSON  (sin comprimir) ó 0x01 | rle...
pub fn compress(raw: &[u8]) -> Vec<u8> {
    // Para claridad y tests: prefijo 0x00 + raw (siempre).
    // Un RLE real iría aquí; el commitment cubre estos bytes.
    let mut out = Vec::with_capacity(1 + raw.len());
    out.push(0x00);
    out.extend_from_slice(raw);
    out
}

pub fn decompress(blob: &[u8]) -> Result<Vec<u8>> {
    if blob.is_empty() {
        return Err(L2Error::InvalidArgument("blob vacío".into()));
    }
    match blob[0] {
        0x00 => Ok(blob[1..].to_vec()),
        _ => Err(L2Error::InvalidArgument("codec DA desconocido".into())),
    }
}

pub fn compress_batch(batch: &Batch) -> Result<Vec<u8>> {
    Ok(compress(&encode_batch(batch)?))
}
