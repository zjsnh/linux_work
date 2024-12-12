#pragma once


#include <sys/epoll.h>
#include <unordered_map>
#include "Log.hpp"
#include <cstring>
#include <utility>
#include <cassert>
#include "Channel.hpp"


namespace muduo
{
    class Channel;
    class Poller
    {
        #define EPOLLEVENTS_MAX 1024
    private:
        int _epfd;
        struct epoll_event _evs[EPOLLEVENTS_MAX];
        std::unordered_map<int, Channel*> _channels;

    public:
        void UpData(Channel* channel,int op)
        {
            int fd = channel->fd();
            struct epoll_event ev;
            ev.data.fd = fd;
            ev.events = channel->Events();

            int n = epoll_ctl(_epfd, op, fd, &ev);
            if(n < 0)
            {
                ERR_LOG("epoll_ctl false : %s", strerror(errno));
            }
            return;
        }

        bool HasChannel(Channel* channel)
        {
            auto it = _channels.find(channel->fd());
            if(it == _channels.end())
                return false;

            return true;
        }

    public:
        Poller()
        {
            _epfd = epoll_create(EPOLLEVENTS_MAX);
            if(_epfd < 0)
            {
                ERR_LOG("epollfd create error:%s ", strerror(errno));
                abort();// 退出程序
            }
        }

        void UpdataEvent(Channel * channel)
        {
            if(HasChannel(channel) == false)
            {
                _channels.insert(std::make_pair(channel->fd(), channel));

                // std::cout << _p.second << std::endl;
                // ERR_LOG("newfd:%d ", channel->fd());
                //std::pair<std::unordered_map<int, Channel*>::iterator,bool> _p = 

                UpData(channel, EPOLL_CTL_ADD);
            }

            UpData(channel, EPOLL_CTL_MOD);
        }

        void RemoveEvent(Channel *channel)
        {
            auto it = _channels.find(channel->fd());
            if (it !=_channels.end())
            {
                _channels.erase(it->first);
            }

            UpData(channel, EPOLL_CTL_DEL);
        }

        void Poll(std::vector<Channel*>* active)
        {
            int nfs = epoll_wait(_epfd, _evs, EPOLLEVENTS_MAX, -1);
            if(nfs < 0)
            {
                if(errno == EINTR) //信号中断
                {
                    return;
                }

                ERR_LOG("EPOLL WAIT ERROR:%s\n", strerror(errno));
                abort();
            }

            for (int i = 0; i < nfs; i++)
            {
                auto it = _channels.find(_evs[i].data.fd);
                //std::cout << "_evs[i].data.fd " << _evs[i].data.fd << "_channels it " << it->second->fd() << std::endl;
                assert(it != _channels.end());
                it->second->SetRrevent(_evs[i].events); //设置实际就绪事件
                active->push_back(it->second);
            }

            return;
        }
    };

    void Channel::Remove() //移除监控
    {
        _poller->RemoveEvent(this);
    }

    void Channel::Updata()
    {
        //std::cout << this->fd() << std::endl;
        _poller->UpdataEvent(this);
    }

}