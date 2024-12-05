#pragma once
#include "socket.hpp"
#include "log.hpp"
#include <poll.h>


const int fd_max_num = 64;
const int defaultfd = -1;

const int non_event = 0;
class PollServer
{
public:
    PollServer(uint16_t port)
        :_port(port)
    {
        for (int i = 0; i < fd_max_num; i++)
        {
            _event_fds[i].fd = -1;   //存放文件描述符
            _event_fds[i].events = non_event;
            _event_fds[i].revents = non_event;
        }
    }

    ~PollServer()
    {
        close(_Listen.Fd());
    }

    void Init()
    {
        _Listen._socket();
        int opt = 1;
        setsockopt(_Listen.Fd(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        _Listen.Bind(_port);
        _Listen.Listen();
    }


    void link()
    {
        sockaddr_in client;
        int sock = _Listen.Accept(client); // 引用
        std::string client_ip = inet_ntoa(client.sin_addr);
        _log(Info, "get a new link ip:%s port:%d", client_ip.c_str(), htons(client.sin_port));

        // 数据处理

        //将新链接文件描述符放到 fd_array里 异步读取
        int pos = 1;
        for (; pos < fd_max_num; pos++)
        {
            if(_event_fds[pos].fd == -1)
            {
                break;
            }
        }

        if(pos == fd_max_num)
        {
            _log(Info, "server full");
            //poll可以进行扩容
        }
        else
        {
            _event_fds[pos].fd = sock;
            _event_fds[pos].events = POLLIN;
        }
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
            close(fd);
            _event_fds[pos].fd = defaultfd;
            _event_fds[pos].events = non_event;
        }
    }

    void trigger()
    {
        for (int i = 0; i < fd_max_num; i++)
        {
            int fd = _event_fds[i].fd;
            if(fd == defaultfd)
                continue;

            if(_event_fds[i].revents & POLLIN)//存在在集合中
            {
                if(fd == _Listen.Fd())
                {
                    link();
                }
                else
                {
                    Recver(fd, i);
                }
            }
        }
    }

    void Run()
    {
        int fd = _Listen.Fd();
        _event_fds[0].fd = fd;
        _event_fds[0].events = POLLIN; //事件
        int timeout = 1;
        while (true)
        {
            //struct timeval timeout{5,0};
            int n = poll(_event_fds,fd_max_num, /*(&timeout)*/ timeout);  // 轮询

            //std::cout << n << std::endl;
            switch (n)
            {
            case 0:
                break;
            case -1:
                _log(Warning, "select error errno %d: %s", errno, strerror(errno));
                break;
            default:
                trigger();
                break;
            }

        }
    }

private:
    Socket _Listen;
    uint16_t _port;

    //int fd_array[fd_max_num]; // 初始化 default -1;
    struct pollfd _event_fds[fd_max_num]; // 数组, 用户维护的！
};