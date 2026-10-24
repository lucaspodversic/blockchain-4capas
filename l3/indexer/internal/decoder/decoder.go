// Package decoder — decodifica transacciones y eventos.
package decoder

import (
	"encoding/hex"
	"strings"
)

type DecodedTx struct {
	From  string
	To    string
	Value string
	Method string
}

// DecodeInput interpreta selector de 4 bytes (ABI simple del SDK).
func DecodeInput(data string) string {
	d := strings.TrimPrefix(data, "0x")
	if len(d) < 8 {
		return "unknown"
	}
	return "sel_" + d[:8]
}

func DecodeAddress(raw string) string {
	raw = strings.TrimPrefix(raw, "0x")
	if len(raw) >= 40 {
		return "0x" + raw[len(raw)-40:]
	}
	b, err := hex.DecodeString(raw)
	if err != nil {
		return "0x" + raw
	}
	return "0x" + hex.EncodeToString(b)
}

func DecodeTx(from, to, value, data string) DecodedTx {
	return DecodedTx{From: from, To: to, Value: value, Method: DecodeInput(data)}
}
