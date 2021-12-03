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
        friend class TcpConnection;

    public:
        TcpServer(std::shared_ptr<EventLoop> loop, SocketAddress address);

        void SetReuseAddr(bool on);
        void SetReusePort(bool on);
        void SetTcpNoDelay(bool on);

        bool BindAndListen();

        void NewConnection(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void ServerError(std::function<void()> callback);

        void OnMessage(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void OnComplete(std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> callback);
        void ClientClose(std::function<void(std::shared_ptr<TcpConnection>)> callback);
        void ClientError(std::function<void(std::shared_ptr<TcpConnection>)> callback);

    private:
        void Accept();
        void EraseConnection(int fd);

    private:
        int fd_;
        SocketAddress address_;
        std::shared_ptr<EventLoop> loop_;
        std::map<int, std::shared_ptr<TcpConnection>> connections_;

        std::function<void(std::shared_ptr<TcpConnection>)> new_connection_callback_;
        std::function<void()> server_error_callback_;

        std::function<void(std::shared_ptr<TcpConnection>)> message_callback_;
        std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> write_complete_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_close_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_error_callback_;
    };
}


#endif //CORE_TCP_SERVER_H
