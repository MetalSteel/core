#include "tcp_connection.h"
#include "tcp_server.h"
#include <unistd.h>

using namespace core;

TcpConnection::TcpConnection(int fd, SocketAddress address, std::shared_ptr<EventLoop> loop, TcpServer *server)
                                : fd_(fd), address_(address), loop_(loop), server_(server) {

    // connection init is read status
    event_.fd = fd_;
    event_.mask = EPOLLIN | EPOLLET;
    event_.read_callback = std::bind(&TcpConnection::Read, this);
    event_.write_callback = std::bind(&TcpConnection::Write, this);
    event_.error_callback = std::bind(&TcpConnection::Error, this);
    loop_->AddEvent(event_);
}

void TcpConnection::Send(std::vector<byte> buffer) {
    // move buffer to write buffer
    write_buffer_ = std::move(buffer);

    // mod epoll event to write status
    event_.mask = EPOLLOUT | EPOLLET;
    loop_->ModEvent(event_);
}

void TcpConnection::Write() {
    // write user mode buffer to tcp kernel buffer
    auto sz = write(fd_, write_buffer_.data(), write_buffer_.size());

    // execute server on write message callback
    if(sz > 0) {
        event_.mask = EPOLLIN | EPOLLET;
        loop_->ModEvent(event_);

        server_->write_complete_callback_(shared_from_this(), sz);
    }
}

void TcpConnection::Read() {
    // read tcp kernel buffer to user mode buffer
    byte buf[1024] = {0};
    auto sz = read(fd_, buf, sizeof(buf));

    // client disconnect from server
    if(sz == 0) {
        // close client connection
        Close();
    }

    if(sz > 0) {
        // execute server on message callback
        read_buffer_.clear();
        read_buffer_.insert(read_buffer_.end(), buf, buf + sz);
        server_->message_callback_(shared_from_this());
    }
}

void TcpConnection::Close() {
    // delete client fd and event from epoll
    loop_->DelEvent(event_);
    close(fd_);

    // execute client close callback function
    server_->client_close_callback_(shared_from_this());

    // erase from server
    server_->EraseConnection(fd_);
}

void TcpConnection::Error() {
    server_->client_error_callback_(shared_from_this());
}
