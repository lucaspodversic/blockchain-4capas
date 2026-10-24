package decoder

import "testing"

func TestDecodeInput(t *testing.T) {
	if DecodeInput("0xa9059cbb00") == "unknown" {
		t.Fatal("expected selector")
	}
}
