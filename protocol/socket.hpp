#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.hpp"

uint16_t _port = 8888;
std::string _ip = "0.0.0.0";
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
    //创建
    Socket(uint16_t port = _port,std::string ip = _ip)
        :port_(port),ip_(ip)
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
    void Bind()
    {
        struct sockaddr_in local;
        local.sin_port = ntohs(port_);
        local.sin_family = AF_INET;
        inet_aton(ip_.c_str(), &(local.sin_addr));

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

    int Accept()
    {
        struct sockaddr_in client;
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

    int Connect(struct sockaddr_in* client,socklen_t len)
    {
        int cnt = 5;
        int isreconnect = false;    

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
        {
            _log(Warning, "sockfd create failed errno:%d %s",errno,strerror(errno));
        }
        do
        {

            int n = connect(sockfd, (sockaddr *)client, len);
            if (n < 0)
            {
                isreconnect = true;
                cnt--;
                std::cerr << "connect error..., reconnect: " << cnt << std::endl;
                sleep(2);
            }
            else
            {
                break;
            }

        } while (cnt && isreconnect);  //重连

        _log(Info, "connect success");
        return sockfd;
    }

private:
    int listening_sockfd_;
    uint16_t port_ ;
    std::string ip_ ;
};