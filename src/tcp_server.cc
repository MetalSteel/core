#include "tcp_server.h"
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>

using namespace core;

TcpServer::TcpServer(std::shared_ptr<EventLoop> loop, SocketAddress address)
                        : loop_(loop), address_(address) {
    // ignore signal SIGPIPE
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal PIPE ignore failed");
        return;
    }

    // create server socket
    fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // check error
    if(fd_ == -1) {
        perror("create server socket failed");
        return;
    }
}

// reuse ip address
void TcpServer::SetReuseAddr(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

// reuse port
void TcpServer::SetReusePort(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
}

// tcp core buffer no delay
void TcpServer::SetTcpNoDelay(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
}

bool TcpServer::BindAndListen() {
    // bind address
    int rt = bind(fd_, address_.SockAddr(), address_.SockLen());
    if (rt == -1) {
        perror("bind tcp server address failed");
        return false;
    }

    // listen socket
    rt = listen(fd_, SOMAXCONN);
    if (rt == -1) {
        perror("listen tcp server failed");
        return false;
    }

    // add server socket fd to epoll
    Event ev;
    ev.fd = fd_;
    ev.mask = EPOLLIN;
    ev.read_callback = std::bind(&TcpServer::Accept, this);
    ev.error_callback = server_error_callback_;

    loop_->AddEvent(ev);

    return true;
}

void TcpServer::Accept() {
    // accept client socket
    SocketAddress cli_addr;
    int cli_fd = accept4(fd_, cli_addr.SockAddr(), &cli_addr.SockLen(), SOCK_NONBLOCK);

    // create client connection and add into connections map
    auto itor = connections_.find(cli_fd);
    if(itor == connections_.end()) {
        // create client connection
        auto connection = std::make_shared<TcpConnection>(cli_fd, cli_addr, loop_, this);

        // add into connections map
        connections_[cli_fd] = connection;

        // execute new connection callback function
        new_connection_callback_(connection);
    }
}

void TcpServer::OnOpen(std::function<void(std::shared_ptr<TcpConnection>)> callback) {
    new_connection_callback_ = callback;
}

void TcpServer::ServerError(std::function<void()> callback) {
    server_error_callback_ = callback;
}

void TcpServer::OnMessage(std::function<void(std::shared_ptr<TcpConnection>)> callback) {
    message_callback_ = callback;
}

void TcpServer::OnComplete(std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> callback) {
    write_complete_callback_ = callback;
}
void TcpServer::OnClose(std::function<void(std::shared_ptr<TcpConnection>)> callback) {
    client_close_callback_ = callback;
}

void TcpServer::OnError(std::function<void(std::shared_ptr<TcpConnection>)> callback) {
    client_error_callback_ = callback;
}

void TcpServer::EraseConnection(int fd) {
    auto itor = connections_.find(fd);
    if(itor != connections_.end()) {
        connections_.erase(itor);
    }
}

void TcpServer::HandleClientClose(std::shared_ptr<TcpConnection> connection) {
    // execute client close callback
    client_close_callback_(connection);

    // erase connection from server
    auto svr = connection->Server();
    svr->EraseConnection(connection->Fd());
}

void TcpServer::HandleClientError(std::shared_ptr<TcpConnection> connection) {
    // execute client error callback
    client_error_callback_(connection);

    // erase connection from server
    auto svr = connection->Server();
    svr->EraseConnection(connection->Fd());
}