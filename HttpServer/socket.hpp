#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.hpp"

const int backlog = 10;

enum socket_error
{
    listening_sockfd_CREATE = 1,
    BIND_ERROR,
    ACCEPT_ERRPR
};

class Socket
{
public:
    Socket()
    {

    }

    ~Socket()
    {

    }
public:
    //创建
    void _socket()
    {
        listening_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if(listening_sockfd_ < 0)
        {
            _log(Fatal,"sockfd create failed erron:%d %s", errno, strerror(errno));
            exit(listening_sockfd_CREATE);
        }

        _log(Info, "sockfd create success ");

    }

    //bind
    void Bind(const uint16_t& port_)
    {
        struct sockaddr_in local;
        local.sin_port = ntohs(port_);
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;

        socklen_t len = sizeof(local);
        if(bind(listening_sockfd_, (const sockaddr *)&local, len) < 0)
        {
            _log(Fatal, "bind fasle errno %d : %s", errno, strerror(errno));
            exit(BIND_ERROR);
        }
        _log(Info, "bind create success ");

    }

    void Listen()
    {
        if(listen(listening_sockfd_,backlog)< 0)
        {
            _log(Fatal, "Listen fasle errno %d : %s", errno, strerror(errno));
            exit(ACCEPT_ERRPR);
        }
        _log(Info, "listen create success ");
    }

    int Accept( struct sockaddr_in& client)
    {
        socklen_t len = sizeof(client);
        int sockfd = accept(listening_sockfd_, (sockaddr *)&(client), &len);
        if(sockfd < 0)
        {
            _log(Fatal, "accept fasle errno %d : %s", errno, strerror(errno));
            exit(ACCEPT_ERRPR);
        }
        _log(Info, "listen create success ");

        return sockfd;
    }

    bool Connect(const std::string &ip, const uint16_t &port)
    {
        struct sockaddr_in peer;
        memset(&peer, 0, sizeof(peer));
        peer.sin_family = AF_INET;
        peer.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &(peer.sin_addr));

        int n = connect(listening_sockfd_, (struct sockaddr*)&peer, sizeof(peer));
        if(n == -1) 
        {
            std::cerr << "connect to " << ip << ":" << port << " error" << std::endl;
            return false;
        }
        return true;
    }

    int Fd()
    {
        return listening_sockfd_;
    }

    void Close()
    {
        close(listening_sockfd_);
    }

private:
    int listening_sockfd_;
};