// =============================================================================
// json_rpc_server.hpp — Servidor JSON-RPC sobre HTTP (sockets POSIX)
// =============================================================================
#pragma once
#include "rpc_methods.hpp"
#include "rpc_auth.hpp"
#include <thread>
#include <atomic>
#include <string>

namespace l1::rpc {

class JsonRpcServer {
public:
    JsonRpcServer(RpcContext ctx, RpcAuth auth = {});
    ~JsonRpcServer();
    core::Result<void> start(const std::string& host, uint16_t port);
    void stop();

private:
    void accept_loop();
    void handle_client(int fd);
    RpcContext ctx_;
    RpcAuth auth_;
    int listen_fd_{-1};
    std::atomic<bool> running_{false};
    std::thread thr_;
};

}  // namespace l1::rpc
