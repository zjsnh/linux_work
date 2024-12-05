#pragma once

#include "nocopy.hpp"
#include <sys/epoll.h>
#include "log.hpp"
#include <cstring>
#include <cerrno>


class Epoller:public nocopy
{
    static const int size = 128;  //红黑树大小

public:
    Epoller() 
    {
        _epollfd = epoll_create(size);
        if(_epollfd == -1)
        {
            _log(Error, "epollfd create error:%s", strerror(errno));
        }
        else
        {
            _log(Info, "epollfd create success");
        }
    }


    int EpollerWait(struct epoll_event revents[],int num)
    {
        int n = epoll_wait(_epollfd, revents, num, -1);//阻塞
        return n;
    }

    int EpollerUpdata(int op,int fd,uint32_t event)
    {
        int n = 0;
        if (op == EPOLL_CTL_DEL)
        {
            n = epoll_ctl(_epollfd, op, fd, NULL);
            if (n != 0)
            {
                _log(Error, "eopll_ctl error");
            }
        }
        else
        {
            //插入，修改
            struct epoll_event ev;
            ev.events = event;
            ev.data.fd = fd;

            n = epoll_ctl(_epollfd, op, fd, &ev);
            if (n != 0)
            {
                _log(Error, "eopll_ctl error");
            }
        }

        return n;
    }

    ~Epoller()
    {
        close(_epollfd);
    }

private:
    int _epollfd;
};