/**
 * Author: Gang
 * Date: 2021-12-03
 * Version: 1.0
 * Description: tcp server only support ipv4
 */

#ifndef CORE_TCP_SERVER_H
#define CORE_TCP_SERVER_H

#include "tcp_connection.h"
#include "event_loop.h"

namespace core {
class TcpServer {
    public:
        TcpServer(std::shared_ptr<EventLoop> loop, SocketAddress address);

        void SetReuseAddr(bool on);
        void SetReusePort(bool on);
        void SetTcpNoDelay(bool on);

        bool BindAndListen();
        void ServerError(std::function<void()> callback);

        void OnOpen(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void OnMessage(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void OnCompleted(std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> callback);
        void OnClose(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void OnError(std::function<void(std::shared_ptr<TcpConnection>)> callback);

    private:
        void Accept();

        void EraseConnection(int fd);
        void HandleReadMessage(std::shared_ptr<TcpConnection> connection);
        void HandleWriteComplete(std::shared_ptr<TcpConnection> connection, ssize_t sz);
        void HandleClientClose(std::shared_ptr<TcpConnection> connection);
        void HandleClientError(std::shared_ptr<TcpConnection> connection);

    private:
        int fd_;
        SocketAddress address_;
        std::shared_ptr<EventLoop> loop_;
        std::map<int, std::shared_ptr<TcpConnection>> connections_;

        std::function<void()> server_error_callback_;

        std::function<void(std::shared_ptr<TcpConnection>)> new_connection_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> read_message_callback_;
        std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> write_completed_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_close_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_error_callback_;
    };
}


#endif //CORE_TCP_SERVER_H
