// Package api — streaming de nuevos datos (WS simplificado / poll hub).
package api

import (
	"sync"
)

// Hub notifica suscriptores cuando hay nuevo tip (sin dependencia websocket en tests).
type Hub struct {
	mu   sync.Mutex
	subs []chan int64
}

func NewHub() *Hub { return &Hub{} }

func (h *Hub) Subscribe() chan int64 {
	ch := make(chan int64, 8)
	h.mu.Lock()
	h.subs = append(h.subs, ch)
	h.mu.Unlock()
	return ch
}

func (h *Hub) Publish(height int64) {
	h.mu.Lock()
	defer h.mu.Unlock()
	for _, ch := range h.subs {
		select {
		case ch <- height:
		default:
		}
	}
}
