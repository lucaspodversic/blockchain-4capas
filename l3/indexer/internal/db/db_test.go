package db

import "testing"

func TestMigrateAndBlock(t *testing.T) {
	d, err := Open(":memory:")
	if err != nil {
		t.Fatal(err)
	}
	if err := d.InsertBlock(BlockRow{Height: 1, Hash: "0x1", Parent: "0x0", StateRoot: "0xr", Timestamp: 1, TxCount: 0}); err != nil {
		t.Fatal(err)
	}
	b, err := d.GetBlock(1)
	if err != nil || b.Hash != "0x1" {
		t.Fatalf("got %+v err=%v", b, err)
	}
	tip, _ := d.Tip()
	if tip != 1 {
		t.Fatalf("tip %d", tip)
	}
}
