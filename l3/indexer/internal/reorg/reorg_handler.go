// Package reorg — maneja reorganizaciones de cadena.
package reorg

import "github.com/blockchain-4capas/l3/indexer/internal/db"

// TruncateFrom borra bloques >= height (y txs asociadas).
func TruncateFrom(d *db.DB, height int64) error {
	if _, err := d.SQL.Exec(`DELETE FROM transactions WHERE block_height >= ?`, height); err != nil {
		return err
	}
	_, err := d.SQL.Exec(`DELETE FROM blocks WHERE height >= ?`, height)
	return err
}

// DetectReorg compara parent del nuevo bloque con tip local.
func DetectReorg(d *db.DB, newHeight int64, parentHash string) (bool, error) {
	if newHeight <= 0 {
		return false, nil
	}
	prev, err := d.GetBlock(newHeight - 1)
	if err != nil {
		return false, nil // sin prev ⇒ no reorg detectable
	}
	return prev.Hash != parentHash, nil
}
