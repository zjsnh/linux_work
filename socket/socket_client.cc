#include <iostream>

#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "log.hpp"


#define ADDR_EXIT 1
#define _SIZE 4096

log _log;


void tips()
{
    cout << "\r\n ./client + 端口 + IP" << endl;
}

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        tips();
        exit(ADDR_EXIT);
    }

    string port_ = argv[1];
    string ip_ = argv[2];

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        _log(Fatal, "sockfd Creation failed errno :%d :%s", errno, strerror(errno));
        exit(ADDR_EXIT);
    }

    _log(Info, "sockfd Creation success errno :%d :%s", errno, strerror(errno));

    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t)atoi(port_.c_str())); // 转化为网络序列
    server.sin_addr.s_addr = inet_addr(ip_.c_str()); // 客户端端设置为 公共IP 给固定IP发送信息
    socklen_t len = sizeof(server);

    std::string msg;
    char buffer[_SIZE];

    while(true)
    {
        cout << "msg : ";
        getline(cin, msg);

        cout << msg << endl;

        if(sendto(sockfd, msg.c_str(), msg.size(), 0, (sockaddr *)&server, len) < 0)
        {
            _log(Fatal, "sendto failed errno :%d :%s", errno, strerror(errno));
        }
        
        _log(Info, "sendto success errno :%d :%s", errno, strerror(errno));

        //接收
        struct sockaddr_in _msg;
        socklen_t len = sizeof(_msg);

        ssize_t s = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (sockaddr *)&_msg, &len);
        if(s > 0)
        {
            buffer[s] = 0;
            cout << buffer << endl;
        }
    }

    return 0;
}