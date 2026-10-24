// =============================================================================
// transport_tcp.hpp — API TCP POSIX
// =============================================================================
#pragma once
#include "messages.hpp"
#include "../core/error.hpp"
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>

namespace l1::p2p {

using MessageHandler = std::function<void(int peer_fd, const Message&)>;

class TcpTransport {
public:
    TcpTransport();
    ~TcpTransport();
    core::Result<void> listen(const std::string& host, uint16_t port);
    core::Result<int> connect(const std::string& host, uint16_t port);
    core::Result<void> send(int fd, const Message& msg);
    void broadcast(const Message& msg);
    void set_handler(MessageHandler h);
    void stop();
    size_t peer_count() const;
private:
    void accept_loop();
    void read_loop(int fd);
    int listen_fd_{-1};
    std::atomic<bool> running_{false};
    std::thread accept_thread_;
    mutable std::mutex mu_;
    std::map<int, core::Bytes> peers_;
    MessageHandler handler_;
};

}  // namespace l1::p2p
