// Package api — API REST (bloques, txs, cuentas, tokens).
package api

import (
	"encoding/json"
	"net/http"
	"strconv"
	"strings"

	"github.com/blockchain-4capas/l3/indexer/internal/db"
	"github.com/blockchain-4capas/l3/indexer/internal/stats"
)

type Server struct {
	DB *db.DB
}

func (s *Server) Handler() http.Handler {
	mux := http.NewServeMux()
	mux.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		writeJSON(w, map[string]string{"status": "ok"})
	})
	mux.HandleFunc("/blocks/", s.getBlock)
	mux.HandleFunc("/stats", s.getStats)
	mux.HandleFunc("/tip", s.getTip)
	return mux
}

func (s *Server) getTip(w http.ResponseWriter, r *http.Request) {
	h, err := s.DB.Tip()
	if err != nil {
		http.Error(w, err.Error(), 500)
		return
	}
	writeJSON(w, map[string]int64{"tip": h})
}

func (s *Server) getBlock(w http.ResponseWriter, r *http.Request) {
	id := strings.TrimPrefix(r.URL.Path, "/blocks/")
	h, err := strconv.ParseInt(id, 10, 64)
	if err != nil {
		http.Error(w, "bad height", 400)
		return
	}
	b, err := s.DB.GetBlock(h)
	if err != nil {
		http.Error(w, "not found", 404)
		return
	}
	writeJSON(w, b)
}

func (s *Server) getStats(w http.ResponseWriter, r *http.Request) {
	st, err := stats.Compute(s.DB)
	if err != nil {
		http.Error(w, err.Error(), 500)
		return
	}
	writeJSON(w, st)
}

func writeJSON(w http.ResponseWriter, v any) {
	w.Header().Set("content-type", "application/json")
	_ = json.NewEncoder(w).Encode(v)
}
