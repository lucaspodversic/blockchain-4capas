// =============================================================================
// signal_handler.hpp — Apagado limpio con SIGINT/SIGTERM
// =============================================================================
#pragma once
#include <atomic>
#include <csignal>

namespace l1::node {

inline std::atomic<bool> g_shutdown{false};

inline void on_signal(int) { g_shutdown.store(true); }

inline void install_signal_handlers() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);
}

}  // namespace l1::node
