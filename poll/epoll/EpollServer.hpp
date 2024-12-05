#pragma once

#include <memory>

#include "socket.hpp"
#include "Epoller.hpp"


class EpollServer
{
    static const int num = 64;

public:
    EpollServer(uint16_t port)
        :_port(port),
        sock_ptr(new Socket()),
        epoll_ptr(new Epoller())
    {}

    void Init()
    {
        sock_ptr->_socket();
        sock_ptr->Bind(_port);
        sock_ptr->Listen();

        _log(Info, "Init success");
    }

     void link()
    {
        sockaddr_in client;
        int sock = sock_ptr->Accept(client); // 引用
        std::string client_ip = inet_ntoa(client.sin_addr);
        _log(Info, "get a new link ip:%s port:%d", client_ip.c_str(), htons(client.sin_port));

        // 数据处理
        epoll_ptr->EpollerUpdata(EPOLL_CTL_ADD, sock, EPOLLIN);
    }

    void Recver(int fd,int pos)
    {
        char buffer[1024];

        int n = read(fd, buffer, sizeof(buffer) - 1);
        if(n > 0)
        {
            buffer[n] = 0;
            std::cout << "msg:" << buffer << std::endl;
        }
        else if(n == 0)
        {
            //断开链接
            _log(Info, "link quit");
            
            epoll_ptr->EpollerUpdata(EPOLL_CTL_DEL, fd, NULL);
            close(fd);
        }
    }

    void trigger(struct epoll_event* revs,int num)
    {
        for (int i = 0; i < num; i++)
        {
            // link
            if(revs[i].data.fd == sock_ptr->Fd())
            {
                link();
            }

            //read
        }
    }

    void run()
    {
        struct epoll_event revs[num];
        int fd = sock_ptr->Fd();
        epoll_ptr->EpollerUpdata(EPOLL_CTL_ADD, fd, EPOLLIN);

        while(true)
        {
            ssize_t n = epoll_ptr->EpollerWait(revs, num);
            if (n > 0)
            {
                //事件就绪
                trigger(revs, n);
            }
        }
    }


    ~EpollServer()
    {
        sock_ptr->Close();
    }

private:
    std::shared_ptr<Socket> sock_ptr;
    std::shared_ptr<Epoller> epoll_ptr;

    uint16_t _port;
};