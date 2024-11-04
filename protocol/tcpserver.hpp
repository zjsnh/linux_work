#pragma once

#include "socket.hpp"
#include <string>
#include <functional>
#include <signal.h>
#include "log.hpp"
#include "protocol.hpp"

using func_t = std::function<std::string(std::string &package)>;

uint16_t _port = 8888;
std::string _ip = "0.0.0.0";

class Server
{
public:
    Server(uint16_t port,func_t callback)
        :callback_(callback),port_(port)
    {
    }

    void Init()
    {
        socket_._socket();
        socket_.Bind(_port);
        socket_.Listen();

        std::cout << "server init success!" << std::endl;
    }

    //多进程
    void Run()
    {
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);

        while(true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = socket_.Accept(clientport,clientip);
            if (sockfd < 0)
                continue;
            _log(Info, "accept a new link, sockfd: %d, clientip: %s, clientport: %d", sockfd, clientip.c_str(), clientport);

            pid_t pid = fork();
            if(pid == 0)
            {
                socket_.Close();
                std::string inbuffer_stream;
                while (true)
                {
                    char buffer[1280];
                    int n = read(sockfd, buffer, sizeof(buffer));
                    if (n > 0)
                    {
                        buffer[n] = 0;
                        inbuffer_stream += buffer;
                        std::cout << inbuffer_stream << endl;
                        while(true)
                        {
                            inbuffer_stream = callback_(inbuffer_stream);
                            if(inbuffer_stream.empty())
                                break;

                            write(sockfd, inbuffer_stream.c_str(), inbuffer_stream.size());
                        }
                    }
                    else
                        break;
                }

                close(sockfd);
                exit(0);
            }

            close(sockfd);
        }
    }


private:
    Socket socket_;
    uint16_t port_;

    func_t callback_;
};