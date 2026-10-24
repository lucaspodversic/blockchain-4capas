package reorg

import (
	"testing"

	"github.com/blockchain-4capas/l3/indexer/internal/db"
)

func TestTruncate(t *testing.T) {
	d, _ := db.Open(":memory:")
	_ = d.InsertBlock(db.BlockRow{Height: 1, Hash: "a", Parent: "0", StateRoot: "r", Timestamp: 1})
	_ = d.InsertBlock(db.BlockRow{Height: 2, Hash: "b", Parent: "a", StateRoot: "r", Timestamp: 2})
	if err := TruncateFrom(d, 2); err != nil {
		t.Fatal(err)
	}
	tip, _ := d.Tip()
	if tip != 1 {
		t.Fatalf("tip %d", tip)
	}
}
