// =============================================================================
// transport_tcp.cpp — Transporte TCP con sockets POSIX
// =============================================================================
#include "transport_tcp.hpp"
#include "codec.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace l1::p2p {

TcpTransport::TcpTransport() = default;
TcpTransport::~TcpTransport() { stop(); }

core::Result<void> TcpTransport::listen(const std::string& host, uint16_t port) {
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) return {core::ErrorCode::NetworkError, "socket"};
    int yes = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (host.empty() || host == "0.0.0.0") addr.sin_addr.s_addr = INADDR_ANY;
    else inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0)
        return {core::ErrorCode::NetworkError, "bind"};
    if (::listen(listen_fd_, 128) < 0) return {core::ErrorCode::NetworkError, "listen"};
    running_ = true;
    accept_thread_ = std::thread([this] { accept_loop(); });
    return core::Result<void>::Ok();
}

core::Result<int> TcpTransport::connect(const std::string& host, uint16_t port) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return {core::ErrorCode::NetworkError, "socket"};
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        ::close(fd);
        return {core::ErrorCode::InvalidArgument, "host"};
    }
    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) {
        ::close(fd);
        return {core::ErrorCode::NetworkError, "connect"};
    }
    { std::lock_guard lock(mu_); peers_[fd] = {}; }
    std::thread([this, fd] { read_loop(fd); }).detach();
    return fd;
}

core::Result<void> TcpTransport::send(int fd, const Message& msg) {
    auto frame = encode_message(msg);
    ssize_t n = ::send(fd, frame.data(), frame.size(), MSG_NOSIGNAL);
    if (n < 0 || static_cast<size_t>(n) != frame.size())
        return {core::ErrorCode::NetworkError, "send"};
    return core::Result<void>::Ok();
}

void TcpTransport::broadcast(const Message& msg) {
    std::vector<int> fds;
    { std::lock_guard lock(mu_); for (auto& [fd, _] : peers_) fds.push_back(fd); }
    for (int fd : fds) send(fd, msg);
}

void TcpTransport::set_handler(MessageHandler h) { handler_ = std::move(h); }

void TcpTransport::stop() {
    running_ = false;
    if (listen_fd_ >= 0) { ::shutdown(listen_fd_, SHUT_RDWR); ::close(listen_fd_); listen_fd_ = -1; }
    if (accept_thread_.joinable()) accept_thread_.join();
    std::lock_guard lock(mu_);
    for (auto& [fd, _] : peers_) ::close(fd);
    peers_.clear();
}

size_t TcpTransport::peer_count() const {
    std::lock_guard lock(mu_);
    return peers_.size();
}

void TcpTransport::accept_loop() {
    while (running_) {
        sockaddr_in cli{};
        socklen_t len = sizeof cli;
        int fd = ::accept(listen_fd_, reinterpret_cast<sockaddr*>(&cli), &len);
        if (fd < 0) continue;
        { std::lock_guard lock(mu_); peers_[fd] = {}; }
        std::thread([this, fd] { read_loop(fd); }).detach();
    }
}

void TcpTransport::read_loop(int fd) {
    core::Bytes buf;
    uint8_t tmp[4096];
    while (running_) {
        ssize_t n = ::recv(fd, tmp, sizeof tmp, 0);
        if (n <= 0) break;
        buf.insert(buf.end(), tmp, tmp + n);
        while (buf.size() >= 4) {
            uint32_t len = static_cast<uint32_t>(buf[0]) |
                           (static_cast<uint32_t>(buf[1]) << 8) |
                           (static_cast<uint32_t>(buf[2]) << 16) |
                           (static_cast<uint32_t>(buf[3]) << 24);
            if (buf.size() < 4ull + len) break;
            core::Bytes frame(buf.begin(), buf.begin() + 4 + len);
            buf.erase(buf.begin(), buf.begin() + 4 + len);
            auto msg = decode_message(frame);
            if (msg && handler_) handler_(fd, msg.value());
        }
    }
    std::lock_guard lock(mu_);
    peers_.erase(fd);
    ::close(fd);
}

}  // namespace l1::p2p
