#include <iostream>

#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "log.hpp"

#define ADDR_EXIT 1
#define SIZE 1024
log _log;
uint16_t _default = 8080;

class Udp_server
{
    public:
        Udp_server(const uint16_t port = _default)   //port ip
            :port_(port)
        {}


        void Init()
        {
            sockfd = socket(AF_INET, SOCK_DGRAM, 0); //   ipv 4   对于UDP  __type -> SOCK_DGRAM
            // SOCK_DGRAM      Supports datagrams (connectionless, unreliable messages of a fixed maximum length).
            if(sockfd < 0)
            {
                _log(Fatal, "sockfd Creation failed errno :%d :%s", errno, strerror(errno));
                exit(ADDR_EXIT);
            }

            _log(Info, "sockfd Creation success errno :%d :%s", errno, strerror(errno));

            // sockaddr_in  结构体创建
            struct sockaddr_in local;
            bzero(&local, sizeof(local));

            local.sin_family = AF_INET;
            local.sin_port = htons(port_); // 转化为网络序列
            local.sin_addr.s_addr = INADDR_ANY; // 服务器端设置为 0 可以收到来自任意ip传入的消息

            socklen_t len = sizeof(local);
            if (bind(sockfd, (const sockaddr *)&local, len) < 0)
            {
                _log(Fatal, "bind failed errno :%d :%s", errno, strerror(errno));
                exit(ADDR_EXIT);
            }
            _log(Info, "bind success errno :%d :%s", errno, strerror(errno));

            _log(Info, "server initiate success errno :%d :%s", errno, strerror(errno));
        }

        void Run()
        {
            char inbuffer[SIZE];
            while(true)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                ssize_t t = recvfrom(sockfd, inbuffer,sizeof(inbuffer) - 1, 0, (sockaddr *)&client, &len);  //接收信息
                if(t < 0)
                {
                    _log(Fatal, "recvfrom failed errno :%d :%s", errno, strerror(errno));
                    continue;
                }

                inbuffer[t] = 0;

                //处理信息
                std::string str = "server to : ";
                str += inbuffer;

                cout << str << endl;

                sendto(sockfd, str.c_str(), str.size(), 0, (const sockaddr *)&client, len);
            }
        }

        ~Udp_server()
        {}


    private:
        int sockfd;
        uint16_t port_; // 端口
        // uint32_t ip_;  // ip 地址
};