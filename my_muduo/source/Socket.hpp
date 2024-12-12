#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <string>
#include "Log.hpp"
#include <unistd.h>
#include <fcntl.h>



namespace muduo
{
    class Socket
    {
        #define LISTEN_MAX 1024
    private:
        int _sockfd;
    public:
        Socket() : _sockfd(-1) {}
        Socket(int fd) : _sockfd(fd) {}
        ~Socket()
        {
            Close();
        }

        int fd() { return _sockfd; }

    public:
        bool Create()
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(_sockfd < 0)
            {
                ERR_LOG("sockfd create failed erron:%d %s", errno, strerror(errno));
                return false;
            }

            INF_LOG("sockfd create success");
            return true;
        }

        bool Bind(const std::string &ip,uint16_t port)
        {
            struct sockaddr_in local;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = inet_addr(ip.c_str());
            local.sin_family = AF_INET;

            socklen_t len = sizeof(struct sockaddr_in);

            if(bind(_sockfd,(const sockaddr*)&(local),len) < 0)
            {
                ERR_LOG("bind failed erron:%d %s", errno, strerror(errno));
                return false;
            }

            INF_LOG("bind failed success");
            return true;
        }

        bool Listen(int backlog = LISTEN_MAX)
        {
            if(listen(_sockfd,backlog) < 0)
            {
                ERR_LOG("Listen failed erron:%d %s", errno, strerror(errno));
                return false;
            }

            INF_LOG("listen failed success");
            return true;
        }

        //向服务器发出链接
        bool Connect(const std::string &ip,uint16_t port)
        {
            struct sockaddr_in local;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = inet_addr(ip.c_str());
            local.sin_family = AF_INET;

            socklen_t len = sizeof(struct sockaddr_in);

            if(connect(_sockfd,(const sockaddr*)&(local),len) < 0)
            {
                ERR_LOG("Connect failed erron:%d %s", errno, strerror(errno));
                return false;
            }
            return true;

        }

        int Accept()
        {
            int newfd = accept(_sockfd, NULL, NULL);
            if (newfd < 0)
            {
                ERR_LOG("Accept failed erron:%d %s", errno, strerror(errno));
            }
            return newfd;
        }

        ssize_t Recv(void* buf,size_t len,int flag = 0) //flag = 0  阻塞
        {
            ssize_t ret = recv(_sockfd, buf, len, flag);
            if(ret <= 0)
            {
                //忽略sock内没有数据，和被信号打断返回的错误
                if(errno == EAGAIN || errno == EINTR)
                    return 0;
                ERR_LOG("Recv failed erron:%d %s", errno, strerror(errno));
                return -1;
            }
            return ret;
        }

        ssize_t NoneBlockRecv(void* buf,size_t len)
        {
            Recv(buf, len, MSG_DONTWAIT);
        }

        ssize_t Send(void* buf,size_t len,int flag)
        {
            ssize_t ret = send(_sockfd, buf, len, flag);
            if(ret <= 0)
            {
                ERR_LOG("Send failed erron:%d %s", errno, strerror(errno));
                return -1;
            }
            return ret;
        }

        ssize_t NoneBlockSend(void* buf,size_t len)
        {
            Send(buf, len, MSG_DONTWAIT);
        }

        void Close()
        {
            if(_sockfd != -1)
            {
                close(_sockfd);
                _sockfd = -1;
            }
        }

        bool CreateServer(uint16_t port,std::string ip = "0.0.0.0" ,int block_flag = true)
        {
            if(Create() == false)
                return false;

            if(block_flag) {
                NonBlock();
            }
            ReuseAddress();

            if(Bind(ip,port) == false)
                return false;

            if(Listen() == false)
                return false;
            

            return true;
        }

        bool CreateClient(uint16_t port,std::string ip)
        {
            if(Create() == false)
                return false;
            
            if(Connect(ip,port))
                return false;

            return true;
        }

        //端口， ip 重用
        void ReuseAddress()
        {
            int val = 1;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&(val), val);
            val = 1;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, (void *)&(val), val);
        }

        //设置套接字非阻塞属性  fcntl
        void NonBlock()
        {
            int flag = fcntl(_sockfd, F_GETFL, 0);
            fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK);
        }
    };
}