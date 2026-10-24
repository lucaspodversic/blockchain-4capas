// =============================================================================
// transport_quic.cpp — Transporte QUIC (fallback a TCP)
// =============================================================================
#include "transport_quic.hpp"
#include <iostream>

namespace l1::p2p {

QuicTransport::QuicTransport() {
    std::cerr << "[l1] QUIC no vinculado; QuicTransport delega a TcpTransport\n";
}
core::Result<void> QuicTransport::listen(const std::string& host, uint16_t port) {
    return tcp_.listen(host, port);
}
core::Result<int> QuicTransport::connect(const std::string& host, uint16_t port) {
    return tcp_.connect(host, port);
}
core::Result<void> QuicTransport::send(int fd, const Message& msg) { return tcp_.send(fd, msg); }
void QuicTransport::broadcast(const Message& msg) { tcp_.broadcast(msg); }
void QuicTransport::set_handler(MessageHandler h) { tcp_.set_handler(std::move(h)); }
void QuicTransport::stop() { tcp_.stop(); }

}  // namespace l1::p2p
