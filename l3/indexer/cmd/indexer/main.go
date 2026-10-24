// Punto de entrada del indexador L3.
package main

import (
	"flag"
	"log"
	"net/http"

	"github.com/blockchain-4capas/l3/indexer/internal/api"
	"github.com/blockchain-4capas/l3/indexer/internal/db"
	"github.com/blockchain-4capas/l3/indexer/internal/fetcher"
)

func main() {
	rpc := flag.String("rpc", "http://127.0.0.1:8545", "JSON-RPC L1/L2")
	addr := flag.String("addr", ":8080", "HTTP listen")
	dsn := flag.String("dsn", ":memory:", "DB DSN (sqlite :memory: o postgres en prod)")
	flag.Parse()

	d, err := db.Open(*dsn)
	if err != nil {
		log.Fatal(err)
	}
	f := fetcher.New(*rpc, d)
	for i := int64(0); i < 3; i++ {
		if err := f.FetchAndStore(i); err != nil {
			log.Printf("fetch %d: %v", i, err)
		}
	}
	srv := &api.Server{DB: d}
	log.Printf("indexer listening on %s", *addr)
	log.Fatal(http.ListenAndServe(*addr, srv.Handler()))
}
