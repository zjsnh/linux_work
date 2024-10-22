#pragma once



#include <iostream>
#include <string>
#include "log.hpp"

extern log _log;

class task
{
public:
    task(int sockfd, const std::string &client_ip, uint16_t client_port)
        :sockfd_(sockfd),client_ip_(client_ip),client_port_(client_port)
    {}

    void run()
    {
        while(true)
        {
            char buffer[4096];

            ssize_t n = read(sockfd_, buffer, sizeof(buffer)); // BUG?
            if (n > 0)
            {
                buffer[n] = 0;
                std::cout << "client key# " << buffer << std::endl;
                std::string echo_string = buffer;

                if(echo_string == "quit")
                {
                    break;
                }

                echo_string += "OK";


                n = write(sockfd_, echo_string.c_str(), echo_string.size()); 
                if(n < 0)
                {
                    _log(Warning, "write error, errno : %d, errstring: %s", errno, strerror(errno));
                }
            }
            else if (n == 0)
            {
                _log(Info, "%s:%d quit, server close sockfd: %d", client_ip_.c_str(), client_port_, sockfd_);
                break;
            }
            else
            {
                _log(Warning, "read error, sockfd: %d, client ip: %s, client port: %d", sockfd_, client_ip_.c_str(), client_port_);
                break;
            }
        }

        
        close(sockfd_);

    }


void operator()()
{
    run();
}

private:
    int sockfd_;
    std::string client_ip_;
    uint16_t client_port_;
};