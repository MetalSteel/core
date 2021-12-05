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
    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
        friend class TcpServer;
    public:
        TcpConnection(int fd, SocketAddress address, std::shared_ptr<EventLoop> loop);

        int Fd() { return fd_; }
        SocketAddress Address() { return address_; }

        std::vector<byte>& ReadBuffer() { return read_buffer_; }
        std::vector<byte>& WriteBuffer() { return write_buffer_; }

        void Send(std::vector<byte> write_buffer_);
        void Close();

    private:
        void Read();
        void Write();
        void Error();

    private:
        int fd_;
        Event event_;
        SocketAddress address_;
        std::shared_ptr<EventLoop> loop_;

        std::vector<byte> read_buffer_;
        std::vector<byte> write_buffer_;

        std::function<void(std::shared_ptr<TcpConnection>)> read_message_callback_;
        std::function<void(std::shared_ptr<TcpConnection>, ssize_t)> write_completed_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_close_callback_;
        std::function<void(std::shared_ptr<TcpConnection>)> client_error_callback_;
    };
}


#endif //CORE_TCP_CONNECTION_H
