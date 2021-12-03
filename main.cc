#include <iostream>
#include "tcp_server.h"
#include "event_loop.h"

using namespace core;

void NewConnection(std::shared_ptr<TcpConnection> connection) {
    std::cout << "new client[" << connection->Address().ToString() << "] connect success." << std::endl;
}

void ServerError() {
    std::cout << "server unknown error." << std::endl;
}

void OnMessage(std::shared_ptr<TcpConnection> connection) {
    auto buffer = connection->ReadBuffer();
    std::cout << "recv data from client[" << connection->Address().ToString() << "]." << std::endl;
    for(ssize_t i = 0; i < buffer.size(); ++i) {
        if(i % 16 == 0) {
            printf("\n");
        }
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    std::cout << "=====================================" << std::endl;

    connection->Send(buffer);
}

void OnComplete(std::shared_ptr<TcpConnection> connection, ssize_t sz) {
    std::cout << std::endl;
    std::cout << "write data from client[" << connection->Address().ToString() << "] size " << sz << std::endl;
    std::cout << std::endl;
}

void ClientClose(std::shared_ptr<TcpConnection> connection) {
    std::cout << "client[" << connection->Address().ToString() << "] disconnect." << std::endl;
}

void ClientError(std::shared_ptr<TcpConnection> connection) {
    std::cout << "client unknown error." << std::endl;
}

int main(int argc, char *argv[]) {

    auto loop = std::make_shared<EventLoop>();

    TcpServer srv(loop, SocketAddress("127.0.0.1:1883"));
    srv.SetReuseAddr(true);
    srv.SetReusePort(true);
    srv.SetTcpNoDelay(true);

    srv.NewConnection(NewConnection);
    srv.ServerError(ServerError);

    srv.OnMessage(OnMessage);
    srv.OnComplete(OnComplete);
    srv.ClientClose(ClientClose);
    srv.ClientError(ClientError);

    srv.BindAndListen();

    loop->Loop();

    return EXIT_SUCCESS;
}
/*
10 5e 00 04 4d 51 54 54 04 ee 00 3c 00 20 37 36
37 31 64 32 34 32 66 65 32 39 34 32 31 37 61 39
32 65 34 61 62 34 31 63 34 35 35 34 34 34 00 13
2f 6f 66 66 6c 69 6e 65 2f 6b 61 79 2d 63 6c 69
65 6e 74 00 0e 64 65 76 69 63 65 20 6f 66 66 6c
69 6e 65 00 03 6b 61 79 00 06 31 32 33 34 35 36

10 5E 00 04 4D 51 54 54 04 EE 00 3C 00 20 37 36
37 31 64 32 34 32 66 65 32 39 34 32 31 37 61 39
32 65 34 61 62 34 31 63 34 35 35 34 34 34 00 13
2F 6F 66 66 6C 69 6E 65 2F 6B 61 79 2D 63 6C 69
65 6E 74 00 0E 64 65 76 69 63 65 20 6F 66 66 6C
69 6E 65 00 03 6B 61 79 00 06 31 32 33 34 35 36
*/
