#include "tcp_connection.h"
#include "tcp_server.h"

#include <unistd.h>

using namespace core;

TcpConnection::TcpConnection(int fd, SocketAddress address, std::shared_ptr<EventLoop> loop)
                                : fd_(fd), address_(address), loop_(loop) {

    // connection init status is read
    event_.fd = fd_;
    event_.mask = EPOLLIN | EPOLLET;
    event_.read_callback = std::bind(&TcpConnection::Read, this);
    event_.write_callback = std::bind(&TcpConnection::Write, this);
    event_.error_callback = std::bind(&TcpConnection::Error, this);
    loop_->AddEvent(event_);
}

void TcpConnection::Read() {
    // empty read buffer
    read_buffer_.clear();

    // read until EAGAIN
    while(true) {
        // read tcp kernel buffer to user mode buffer
        byte buf[1024] = {0};
        auto read_sz = read(fd_, buf, sizeof(buf));

        if(read_sz > 0 && read_sz < 1024) {
            read_buffer_.insert(read_buffer_.end(), buf, buf + read_sz);
            read_message_callback_(shared_from_this());
            break;
        }
        else if(read_sz == 0) {
            // close client connection
            Close();
            break;
        }
        else if(read_sz == 1024) {
            // there is still data readable
            read_buffer_.insert(read_buffer_.end(), buf, buf + read_sz);
        }
        else if(read_sz == -1) {
            if(errno == EAGAIN) {
                // read data is empty
                read_message_callback_(shared_from_this());
                break;
            }
            else if(errno == EINTR) {
                // read data interrupt
                perror("read data from client failed");
                break;
            }
            else {
                perror("read data from client failed");
                break;
            }
        }
        else {
            perror("read data from client failed");
            break;
        }
    }
}

void TcpConnection::Write() {
    auto total_sz = write_buffer_.size();
    auto write_sz = 0;

    // write data until end
    while(write_sz != total_sz) {
        auto n = write(fd_, write_buffer_.data(), write_buffer_.size());
        if(n == -1) {
            if(errno != EAGAIN || errno != EINTR) {
                perror("write data to client failed");
                return;
            } else {
                continue;
            }
        }
        write_sz += n;
    }

    // mod epoll event to write status
    event_.mask = EPOLLIN | EPOLLET;
    loop_->ModEvent(event_);

    // execute server on write message callback
    write_completed_callback_(shared_from_this(), write_sz);
}

void TcpConnection::Error() {
    // delete client fd event from epoll
    loop_->DelEvent(event_);

    // close socket fd
    close(fd_);

    // handle client error
    client_error_callback_(shared_from_this());
}

void TcpConnection::Send(std::vector<byte> buffer) {
    // move buffer to write buffer
    write_buffer_ = std::move(buffer);

    // mod epoll event to write status
    event_.mask = EPOLLOUT | EPOLLET;
    loop_->ModEvent(event_);
}

void TcpConnection::Close() {
    // delete client fd and event from epoll
    loop_->DelEvent(event_);

    // close socket
    close(fd_);

    // handle client close
    client_close_callback_(shared_from_this());
}