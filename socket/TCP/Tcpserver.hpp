#pragma once
#include <iostream>

#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"

uint16_t _port = 8888;
std::string _ip = "0.0.0.0";
const int backlog = 10;

extern log _log;

enum _error
{
    LISTERN_SOCKFD = 1,
    ACCEPT_ERRPR,
    BIND_ERROR
};

class TCP_server
{
public:
    TCP_server(const uint16_t& port = _port,const std::string& ip =_ip )
        :port_(port),server_ip(ip)
    {}

    void Init()
    {
        listen_sockfd_ = socket(AF_INET, SOCK_STREAM, 0); //  ipv4  stream
        if(listen_sockfd_ < 0)
        {
            _log(Fatal, "listen_sockfd create fasle errno %d : %s", errno, strerror(errno));
            exit(LISTERN_SOCKFD);
        }
        _log(Info, "listen_sockfd create success errno ");

        struct sockaddr_in local;
        local.sin_port = ntohs(port_);
        local.sin_family = AF_INET;
        inet_aton(server_ip.c_str(), &(local.sin_addr));

        socklen_t len = sizeof(local);
        
        if(bind(listen_sockfd_, (const sockaddr *)&local, len) < 0)
        {
            _log(Fatal, "bind fasle errno %d : %s", errno, strerror(errno));
            exit(BIND_ERROR);
        }
        _log(Info, "bind create success ");


        if(listen(listen_sockfd_,backlog)< 0)
        {
            _log(Fatal, "accept fasle errno %d : %s", errno, strerror(errno));
            exit(ACCEPT_ERRPR);
        }
        _log(Info, "listen create success ");


        _log(Info, "server initiate success ");
    }

    void Run()
    {
        ThreadPool<task>::GetInstance()->Start();
        _log(Info, "server running ...");
        while(true)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = accept(listen_sockfd_, (sockaddr *)&(client), &len); //新连接的sockfd 

            //创建任务
            std::string client_ip = inet_ntoa(client.sin_addr);
            uint16_t client_port = htons(client.sin_port);
            task t(sockfd, client_ip, client_port);

            ThreadPool<task>::GetInstance()->Push(t);
        }
    }


    ~TCP_server()
    {}


private:
    uint16_t port_;
    std::string server_ip;   // 0.0.0.0
    int listen_sockfd_;
};