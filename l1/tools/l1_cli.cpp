// =============================================================================
// l1_cli.cpp — CLI de administración del nodo (consulta JSON-RPC)
// =============================================================================
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

static std::string rpc_call(const std::string& host, uint16_t port, const std::string& body) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return "";
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) {
        close(fd);
        return "";
    }
    std::ostringstream req;
    req << "POST / HTTP/1.1\r\nHost: " << host << "\r\nContent-Type: application/json\r\n"
        << "Content-Length: " << body.size() << "\r\nConnection: close\r\n\r\n" << body;
    auto s = req.str();
    send(fd, s.data(), s.size(), MSG_NOSIGNAL);
    char buf[65536];
    std::string resp;
    ssize_t n;
    while ((n = recv(fd, buf, sizeof buf, 0)) > 0) resp.append(buf, buf + n);
    close(fd);
    auto p = resp.find("\r\n\r\n");
    return p == std::string::npos ? resp : resp.substr(p + 4);
}

int main(int argc, char** argv) {
    std::string host = "127.0.0.1";
    uint16_t port = 8545;
    std::string method = "l1_blockNumber";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--rpc" && i + 1 < argc) {
            std::string ep = argv[++i];
            auto c = ep.find(':');
            if (c != std::string::npos) {
                host = ep.substr(0, c);
                port = static_cast<uint16_t>(std::stoi(ep.substr(c + 1)));
            }
        } else if (std::string(argv[i]) == "--method" && i + 1 < argc) {
            method = argv[++i];
        } else if (std::string(argv[i]) == "blockNumber") {
            method = "l1_blockNumber";
        } else if (std::string(argv[i]) == "chainId") {
            method = "l1_chainId";
        }
    }
    std::string body = "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"" + method + "\",\"params\":[]}";
    std::cout << rpc_call(host, port, body) << "\n";
    return 0;
}
