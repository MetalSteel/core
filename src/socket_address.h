/**
 * Author: Gang
 * Date: 2021-12-03
 * Version: 1.0
 * Description: socket address only support ipv4
 */

#ifndef CORE_SOCKET_ADDRESS_H
#define CORE_SOCKET_ADDRESS_H

#include <arpa/inet.h>
#include <string>

namespace core {
    // socket address
    class SocketAddress {
    public:
        SocketAddress();
        SocketAddress(std::string addr);

        sockaddr* SockAddr();
        socklen_t& SockLen();

        std::string IP() { return ip_; }
        uint16_t Port() { return port_; }

        std::string ToString();
    private:
        std::string ip_;
        uint16_t port_;
        sockaddr_in ipv4_;
        socklen_t ipv4_len_;
    };
}

#endif //CORE_SOCKET_ADDRESS_H
