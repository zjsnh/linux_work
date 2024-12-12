#include "Buffer.hpp"
#include <iostream>
#include <algorithm>
#include "Socket.hpp"
#include <unistd.h>
#include "Poller.hpp"
#include "timewheel.hpp"


void Buf()
{
    muduo::Buffer buf;
    for (int i = 0; i < 300; i++)
    {
        std::string str = "hello " + std::to_string(i) + '\n';
        buf.WriteAndPushString(str);
    }

    std::cout << buf.ReadAbleSize() << std::endl;
    for (int i = 0; i < 3; i++)
    {
        std::string str = buf.GetLineAndPop();
        std::cout << str;
    }

    std::cout << buf.ReadAbleSize() << std::endl;

    // std::string str = "hello";
    // Buffer buf;
    // buf.WriteAndPushString(str);

    // std::cout << buf.ReadAsString(buf.ReadAbleSize()) << std::endl;
    // std::cout << buf.ReadAbleSize() << std::endl;

    // //std::string _str = "nihao ";
    // // _str += buf.ReadAsString(1);
    // // std::cout << _str << std::endl;
}

void Sok()
{
    muduo::Socket sock;
    sock.CreateServer(8888); 

    while(1)
    {
        if(sock.Accept() > 0)
        {
            INF_LOG("get a new accept");
        }
        else{
            INF_LOG("wait a new accept");
        }
        sleep(1);
    }
}

void ReadCb(muduo::Channel* channel)
{
    int fd = channel->fd();
    char buf[1024] = {0};
    int n = read(fd, buf, sizeof(buf));
    if(n <= 0)
        return;
    buf[n] = '\0';

    std::cout << buf ;
}

void Listen(muduo::Poller* poll,muduo::Channel* channel)
{
    int fd = channel->fd();

    int n = accept(fd, NULL, NULL);
    if(n < 0)
        return;

    muduo::Channel *newcha = new muduo::Channel(poll, n);
    //newcha 设置回调函数
    newcha->EnAbleRead();
    newcha->SetReadCb(std::bind(&ReadCb, newcha));

    //poll->UpdataEvent(newcha);
}

void Cha()
{
    muduo::Socket sock;
    sock.CreateServer(8888); // 非阻塞 blog_flag = 1;

    muduo::Poller poll;
    muduo::Channel cha(&poll, sock.fd());
    cha.EnAbleRead();  //可读
    cha.SetReadCb(std::bind(&Listen, &poll, &cha));
    std::vector<muduo::Channel *> actives;

    // std::cout <<"Listen " <<reinterpret_cast<uintptr_t>(&Listen) << std::endl;
    // std::cout <<"ReadCb " <<reinterpret_cast<uintptr_t>(&ReadCb) << std::endl;
    // std::cout <<"Listen " <<(void*)(&Listen) << std::endl;
    // std::cout << "ReadCb " << (void *)(&ReadCb) << std::endl;

    while (true)
    {
        poll.Poll(&actives);
        if (!actives.empty())
        {
            for(auto& e:actives)
            {
                e->HandleEvent();
            }
        }
    }
}

int main()
{
    Cha();

    return 0;
}