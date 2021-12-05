/**
 * Author: Gang
 * Date: 2021-12-03
 * Version: 1.0
 * Description: tcp connection
 */

#ifndef CORE_TCP_CONNECTION_H
#define CORE_TCP_CONNECTION_H

#include <vector>
#include <memory>

#include "misc.h"
#include "socket_address.h"
#include "event_loop.h"

namespace core {
    class TcpServer;

    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    public:
        TcpConnection(int fd, SocketAddress address, std::shared_ptr<EventLoop> loop, TcpServer *server);

        void Send(std::vector<byte> write_buffer_);
        void Close();

        int Fd() { return fd_; }
        SocketAddress Address() { return address_; }
        TcpServer* Server() { return server_; }

        std::vector<byte>& ReadBuffer() { return read_buffer_; }
        std::vector<byte>& WriteBuffer() { return write_buffer_; }

    private:
        void Write();
        void Read();
        void Error();

    private:
        int fd_;
        Event event_;
        SocketAddress address_;
        std::shared_ptr<EventLoop> loop_;
        TcpServer *server_;

        std::vector<byte> read_buffer_;
        std::vector<byte> write_buffer_;
    };
}


#endif //CORE_TCP_CONNECTION_H
