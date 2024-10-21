#include <iostream>

#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "log.hpp"


#define ADDR_EXIT 1
#define _SIZE 4096

log _log;


void tips()
{
    cout << "\r\n ./client + 端口 + IP" << endl;
}

//socket 支持同时输入输出  多线程

struct ThreadData
{
    struct sockaddr_in server;
    int sockfd;
    std::string serverip;
};

//使用 标准错误输出 ->  多个终端实现输入输出分离
void* Recv(void* args)
{

    ThreadData *td = static_cast<ThreadData *>(args);
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));  //
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        ssize_t s = recvfrom(td->sockfd, buffer, 1023, 0, (struct sockaddr *)&temp, &len);
        if (s > 0)
        {
            buffer[s] = 0;
            cerr << buffer << endl;
        }
    }

}

void* Send(void* args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    string message;
    socklen_t len = sizeof(td->server);

    std::string welcome = td->serverip;
    welcome += " comming...";
    sendto(td->sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&(td->server), len);

    while (true)
    {
        cout << "Please Enter@ ";
        getline(cin, message);
        
        // std::cout << message << std::endl;
        // 1. 数据 2. 给谁发
        sendto(td->sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&(td->server), len);
    }
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

    // server.sin_family = AF_INET;
    // server.sin_port = htons((uint16_t)atoi(port_.c_str())); // 转化为网络序列
    // server.sin_addr.s_addr = inet_addr(ip_.c_str()); // 客户端端设置为 公共IP 给固定IP发送信息
    // socklen_t len = sizeof(server);

    struct ThreadData td;
    bzero(&td.server, sizeof(td.server));
    td.server.sin_family = AF_INET;
    td.server.sin_port = htons((u_int16_t)atoi(port_.c_str())); //?
    td.server.sin_addr.s_addr = inet_addr(ip_.c_str());

    td.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (td.sockfd < 0)
    {
        cout << "socker error" << endl;
        return 1;
    }

    td.serverip = ip_;

    pthread_t recvr, sender;
    pthread_create(&recvr, nullptr, Recv, &td);
    pthread_create(&sender, nullptr, Send, &td);

    pthread_join(recvr, nullptr);
    pthread_join(sender, nullptr);

    return 0;
}