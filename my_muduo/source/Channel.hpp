#pragma once

#include <functional>
#include <sys/epoll.h>
#include "Poller.hpp"

namespace muduo
{
    class Poller;
    class Channel
    {
        using EventCallBack = std::function<void()>;
    private:
        int _fd;
        Poller *_poller;
        uint32_t _event;
        uint32_t _revent;
        EventCallBack _read_cb;  //读
        EventCallBack _write_cb; //写
        EventCallBack _error_cb; //错误
        EventCallBack _close_cb; //关闭
        EventCallBack _event_cb; //任意

    public:
        Channel(Poller* poller ,int fd) : _poller(poller) ,_fd(fd), _event(0), _revent(0) {}
        int fd() { return _fd; }

        void SetRrevent(uint32_t event) { _revent = event; }
        uint32_t Events() { return _event; }

        void SetReadCb(const EventCallBack &read_cb) { _read_cb = read_cb; }
        void SetwriteCb(const EventCallBack &write_cb) { _read_cb = write_cb; }
        void SetErrorCb(const EventCallBack &error_cb) { _error_cb = error_cb; }
        void SetCloseCb(const EventCallBack &close_cb) { _close_cb = close_cb; }
        void SetEventCb(const EventCallBack &event_cb) { _event_cb = event_cb; }

        bool ReadVble() { return (_event & EPOLLIN); }
        bool WriteAble() { return (_event & EPOLLOUT); }

        //更新epoll内数据  updata()
        void EnAbleRead() { (_event |= EPOLLIN); Updata(); }
        void EnAbleWrite() { (_event |= EPOLLOUT); Updata();}
        void DisableRead() { (_event &= ~EPOLLIN); Updata();}
        void DisableWrite() { (_event &= ~EPOLLOUT); Updata();}
        void DisableAll() { _event = 0; }

        void Remove(); // 移除监控
        void Updata();

        void HandleEvent()
        {

            if((_revent&EPOLLIN)||(_revent&EPOLLRDBAND)||(_revent&EPOLLERR))
            {   if(_event_cb)
                    _event_cb();
                if(_read_cb)
                {
                    _read_cb();
                }
            }

            //可能会断开链接的操作
            if((_revent & EPOLLOUT))
            {
                if(_write_cb)
                    _write_cb();
                if(_event_cb)
                    _event_cb();
            }
            else if(_revent & EPOLLERR)
            {
                if(_event_cb)
                    _event_cb();
                if(_event_cb)
                    _event_cb();
            }
            else if(_revent & EPOLLHUP) //关闭连接
            {
                if(_event_cb)
                    _event_cb();
                if(_close_cb)
                    _close_cb();
            }
        }
    };

}
