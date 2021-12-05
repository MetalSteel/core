#include <iostream>
#include "tcp_server.h"
#include "event_loop.h"

using namespace core;

class EchoServer {
public:
    EchoServer(std::string addr) : loop_(std::make_shared<EventLoop>()), server_(loop_, addr) {
        server_.SetReuseAddr(true);
        server_.SetReusePort(true);
        server_.SetTcpNoDelay(true);

        server_.ServerError(std::bind(&EchoServer::ServerError, this));

        server_.OnOpen(std::bind(&EchoServer::OnOpen, this, std::placeholders::_1));
        server_.OnMessage(std::bind(&EchoServer::OnMessage, this, std::placeholders::_1));
        server_.OnComplete(std::bind(&EchoServer::OnComplete, this, std::placeholders::_1, std::placeholders::_2));
        server_.OnClose(std::bind(&EchoServer::OnClose, this, std::placeholders::_1));
        server_.OnError(std::bind(&EchoServer::OnError, this, std::placeholders::_1));
    }

    void Start() {
        server_.BindAndListen();
        loop_->Loop();
    }

private:
    void OnOpen(std::shared_ptr<TcpConnection> connection) {
        std::cout << "new client[" << connection->Address().ToString() << "] connect success." << std::endl;
    }

    void ServerError() {
        std::cout << "server unknown error." << std::endl;
    }

    void OnMessage(std::shared_ptr<TcpConnection> connection) {
        auto buffer = connection->ReadBuffer();
        std::cout << "recv data from client[" << connection->Address().ToString() << "] size " << buffer.size() << std::endl;
        connection->Send(buffer);
    }

    void OnComplete(std::shared_ptr<TcpConnection> connection, ssize_t sz) {
        std::cout << "write data to client[" << connection->Address().ToString() << "] size " << sz << std::endl;
    }

    void OnClose(std::shared_ptr<TcpConnection> connection) {
        std::cout << "client[" << connection->Address().ToString() << "] disconnect." << std::endl;
    }

    void OnError(std::shared_ptr<TcpConnection> connection) {
        std::cout << "client unknown error." << std::endl;
    }

private:
    std::shared_ptr<EventLoop> loop_;
    TcpServer server_;
};

int main(int argc, char *argv[]) {
    EchoServer server("127.0.0.1:9876");
    server.Start();

    return EXIT_SUCCESS;
}