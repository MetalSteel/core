#include "socket_address.h"
#include <strings.h>

using namespace core;

SocketAddress::SocketAddress() : ip_(""), port_(0) {
    bzero(&ipv4_, sizeof(ipv4_));
    ipv4_len_ = sizeof(ipv4_);
}

SocketAddress::SocketAddress(std::string addr) {
    auto index = addr.find(':');
    ip_ = addr.substr(0, index);
    port_ = static_cast<uint16_t>(stoi(addr.substr(index + 1, addr.size())));

    ipv4_.sin_family = AF_INET;
    inet_pton(AF_INET, ip_.c_str(), &ipv4_.sin_addr);
    ipv4_.sin_port = htons(port_);

    ipv4_len_ = sizeof(ipv4_);
}

sockaddr* SocketAddress::SockAddr() {
    return reinterpret_cast<sockaddr *>(&ipv4_);
}

socklen_t& SocketAddress::SockLen() {
    return ipv4_len_;
}

std::string SocketAddress::ToString() {
    char ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &ipv4_.sin_addr, ip, INET_ADDRSTRLEN);
    return std::string(std::string(ip) + ":" + std::to_string(ntohs(ipv4_.sin_port)));
}
