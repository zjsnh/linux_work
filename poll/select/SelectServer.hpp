#pragma once
#include "socket.hpp"
#include "log.hpp"
#include <sys/select.h>


const int fd_max_num = sizeof(fd_set) * 8;
const int defaultfd = -1;
class SelectServer
{
public:
    SelectServer(uint16_t port)
        :_port(port)
    {
        for (int i = 0; i < fd_max_num; i++)
        {
            fd_array[i] = -1;   //存放文件描述符
        }
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
            if(fd_array[pos] == -1)
            {
                break;
            }
        }

        if(pos == fd_max_num)
        {
            _log(Info, "server full");
        }
        else
        {
            fd_array[pos] = sock;
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
            fd_array[pos] = -1;
        }
    }

    void trigger(fd_set& rfds)
    {
        for (int i = 0; i < fd_max_num; i++)
        {
            if(FD_ISSET(fd_array[i], &rfds))
            {
                if(fd_array[i]==_Listen.Fd())
                {
                    link();
                }
                else
                {
                    Recver(fd_array[i], i);
                }
            }
        }

    }

    void Run()
    {
        int fd = _Listen.Fd();
        fd_array[0] = fd;
        while (true)
        {
            //std::cout << fd_array[0] << std::endl;
            fd_set rfds;
            FD_ZERO(&rfds);

            int max_fd = fd;
            for (int i = 0; i < fd_max_num; i++)
            {
                if (fd_array[i] != defaultfd)
                {
                    if (fd < fd_array[i])
                        max_fd = fd_array[i];
                    FD_SET(fd_array[i], &rfds);
                }
            }

                
            //struct timeval timeout{5,0};
            int n = select(max_fd + 1, &rfds, NULL, NULL, /*(&timeout)*/ NULL);  // 阻塞等待

            //std::cout << n << std::endl;
            switch (n)
            {
            case 0:
                break;
            case -1:
                _log(Warning, "select error errno %d: %s", errno, strerror(errno));
                break;
            default:
                trigger(rfds);
                break;
            }

        }
    }

private:
    Socket _Listen;
    uint16_t _port;

    int fd_array[fd_max_num]; // 初始化 default -1;
};