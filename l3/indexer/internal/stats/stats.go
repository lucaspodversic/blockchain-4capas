// Package stats — TPS, usuarios activos, TVL.
package stats

import "github.com/blockchain-4capas/l3/indexer/internal/db"

type Snapshot struct {
	Tip        int64   `json:"tip"`
	TxTotal    int64   `json:"tx_total"`
	TPS        float64 `json:"tps"`
	ActiveAddr int64   `json:"active_addresses"`
}

func Compute(d *db.DB) (Snapshot, error) {
	var s Snapshot
	tip, err := d.Tip()
	if err != nil {
		return s, err
	}
	s.Tip = tip
	_ = d.SQL.QueryRow(`SELECT COUNT(*) FROM transactions`).Scan(&s.TxTotal)
	_ = d.SQL.QueryRow(`SELECT COUNT(DISTINCT from_addr) FROM transactions`).Scan(&s.ActiveAddr)
	if tip > 0 {
		s.TPS = float64(s.TxTotal) / float64(tip) // aproximación por bloque
	}
	return s, nil
}
