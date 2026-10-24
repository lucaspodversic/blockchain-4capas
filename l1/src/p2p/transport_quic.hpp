// =============================================================================
// transport_quic.hpp — API QUIC (fallback TCP)
// =============================================================================
#pragma once
#include "transport_tcp.hpp"
namespace l1::p2p {
class QuicTransport {
public:
    QuicTransport();
    core::Result<void> listen(const std::string& host, uint16_t port);
    core::Result<int> connect(const std::string& host, uint16_t port);
    core::Result<void> send(int fd, const Message& msg);
    void broadcast(const Message& msg);
    void set_handler(MessageHandler h);
    void stop();
private:
    TcpTransport tcp_;
};
}  // namespace l1::p2p
