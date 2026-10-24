// =============================================================================
// json_rpc_server.cpp — HTTP mínimo POST / + JSON-RPC body
// =============================================================================
#include "json_rpc_server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <cstring>

namespace l1::rpc {

JsonRpcServer::JsonRpcServer(RpcContext ctx, RpcAuth auth)
    : ctx_(std::move(ctx)), auth_(std::move(auth)) {}

JsonRpcServer::~JsonRpcServer() { stop(); }

core::Result<void> JsonRpcServer::start(const std::string& host, uint16_t port) {
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
        return {core::ErrorCode::NetworkError, "bind rpc"};
    if (::listen(listen_fd_, 64) < 0) return {core::ErrorCode::NetworkError, "listen rpc"};
    running_ = true;
    thr_ = std::thread([this] { accept_loop(); });
    return core::Result<void>::Ok();
}

void JsonRpcServer::stop() {
    running_ = false;
    if (listen_fd_ >= 0) { ::shutdown(listen_fd_, SHUT_RDWR); ::close(listen_fd_); listen_fd_ = -1; }
    if (thr_.joinable()) thr_.join();
}

void JsonRpcServer::accept_loop() {
    while (running_) {
        int fd = ::accept(listen_fd_, nullptr, nullptr);
        if (fd < 0) continue;
        std::thread([this, fd] { handle_client(fd); }).detach();
    }
}

void JsonRpcServer::handle_client(int fd) {
    char buf[65536];
    ssize_t n = ::recv(fd, buf, sizeof buf - 1, 0);
    if (n <= 0) { ::close(fd); return; }
    buf[n] = 0;
    std::string req(buf, buf + n);
    // Auth header
    auto pos = req.find("Authorization:");
    if (auth_.require_auth()) {
        std::string header;
        if (pos != std::string::npos) {
            auto end = req.find("\r\n", pos);
            header = req.substr(pos + 14, end - pos - 14);
            while (!header.empty() && header[0] == ' ') header.erase(0, 1);
        }
        if (!auth_.check_bearer(header)) {
            std::string resp = "HTTP/1.1 401 Unauthorized\r\nContent-Length: 0\r\n\r\n";
            ::send(fd, resp.data(), resp.size(), MSG_NOSIGNAL);
            ::close(fd);
            return;
        }
    }
    auto body_pos = req.find("\r\n\r\n");
    std::string body = body_pos == std::string::npos ? req : req.substr(body_pos + 4);
    nlohmann::json jreq;
    nlohmann::json jresp;
    try {
        jreq = nlohmann::json::parse(body);
        jresp = dispatch_rpc(ctx_, jreq);
    } catch (const std::exception& ex) {
        jresp = nlohmann::json{{"jsonrpc","2.0"},{"id",nullptr},
            {"error",{{"code",-32700},{"message",ex.what()}}}};
    }
    std::string payload = jresp.dump();
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
        << payload.size() << "\r\nConnection: close\r\n\r\n" << payload;
    auto resp = oss.str();
    ::send(fd, resp.data(), resp.size(), MSG_NOSIGNAL);
    ::close(fd);
}

}  // namespace l1::rpc
