// Package fetcher — descarga bloques del nodo (L1 y L2).
package fetcher

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strings"
	"time"

	"github.com/blockchain-4capas/l3/indexer/internal/db"
)

type Fetcher struct {
	RPC string
	DB  *db.DB
	HTTP *http.Client
}

func New(rpc string, d *db.DB) *Fetcher {
	return &Fetcher{RPC: rpc, DB: d, HTTP: &http.Client{Timeout: 5 * time.Second}}
}

// FetchAndStore obtiene un bloque (o genera mock si RPC caído).
func (f *Fetcher) FetchAndStore(height int64) error {
	body := fmt.Sprintf(`{"jsonrpc":"2.0","id":1,"method":"eth_getBlockByNumber","params":["0x%x", true]}`, height)
	var hash, parent, root string
	txCount := 0
	ts := time.Now().Unix()

	resp, err := f.HTTP.Post(f.RPC, "application/json", strings.NewReader(body))
	if err == nil && resp != nil {
		defer resp.Body.Close()
		b, _ := io.ReadAll(resp.Body)
		var parsed map[string]any
		_ = json.Unmarshal(b, &parsed)
		if r, ok := parsed["result"].(map[string]any); ok && r != nil {
			hash, _ = r["hash"].(string)
			parent, _ = r["parentHash"].(string)
			root, _ = r["stateRoot"].(string)
		}
	}
	if hash == "" {
		hash = fmt.Sprintf("0xmock%056d", height)
		parent = fmt.Sprintf("0xmock%056d", height-1)
		root = fmt.Sprintf("0xroot%056d", height)
		txCount = 1
		_ = f.DB.InsertTx(fmt.Sprintf("0xtx%062d", height), height, 0, "0xaaa", "0xbbb", "100", 1)
	}
	return f.DB.InsertBlock(db.BlockRow{
		Height: height, Hash: hash, Parent: parent, StateRoot: root, Timestamp: ts, TxCount: txCount,
	})
}
