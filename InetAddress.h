#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, const std::string& ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in& addr)
    :  addr_(addr)
    {}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in* getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in& addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};