#include <iostream>

#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unordered_map>
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
            local.sin_port = htons(port_);      // 转化为网络序列
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

        void Online_User(const struct sockaddr_in& client,const string& IP,u_int16_t port)
        {

            if(clinet_ip.find(IP)==clinet_ip.end())
            {
                clinet_ip.insert(make_pair(IP, client));
                _log(Info, "[%s : %d]"
                           "User join success errno :%d :%s",
                     IP.c_str(), port, errno, strerror(errno));
            }
        }

        void BroadCast(const string& message,const string& IP,u_int16_t port)
        {
            string Forward;   //  [ip:port] : message
            for (const auto &e : clinet_ip)
            {
                Forward += "[";
                Forward += IP;
                Forward += ":";
                Forward += "] :";

                Forward += message;
                socklen_t len = sizeof(e.second);

                sendto(sockfd, Forward.c_str(), Forward.size(), 0, (const sockaddr *)&(e.second), len);
            }
        }

        void Run()
        {
            cout << "running !" << endl;

            std::string ip ;
            u_int16_t port;

            char inbuffer[SIZE];

            string message;
            while (true)
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

                message = inbuffer;

                ip = inet_ntoa(client.sin_addr);
                port = client.sin_port;

                Online_User(client, ip, port);   //在线用户表
                //处理信息 转发

                
                BroadCast(message,ip,port);
                
            }
        }

        ~Udp_server()
        {}


    private:
        int sockfd;
        uint16_t port_; // 端口
        // uint32_t ip_;  // ip 地址

        unordered_map<std::string, struct sockaddr_in> clinet_ip;
};