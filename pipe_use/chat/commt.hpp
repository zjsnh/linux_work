#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include "log.hpp"
#include <sys/wait.h>



#define FIFO_FILE "log.txt"
#define TIME_OUT 5
#define CIN_TIMES 3

#define SEND "send.txt"
#define REVERSE "reverse.txt"

log _log;  //全局 -> server段不必再次定义

class Init
{
    public:
        Init()
        {
            int send = mkfifo(Sending_channel.c_str(), 0664);
            if(send < 0)
            {
                _log(Fatal,"Sending_channel create fail:");
                exit(-1);
            }
            
            int receive = mkfifo(Receiving_channel.c_str(), 0664);
            if(receive < 0)
            {
                _log(Fatal,"Receiving_channel create fail:");
                exit(-1);
            }

        }


        ~Init()
        {
            int send = unlink(Sending_channel.c_str());
            if(send < 0)
            {
                _log(Fatal, "Sending_channel close fail:");
                exit(-1);
            }

            int receive = unlink(Receiving_channel.c_str());
            if(receive < 0)
            {
                _log(Fatal, "Receiving_channel close fail:");
                exit(-1);
            }
        }

    private:
        std::string Sending_channel = SEND;
        std::string Receiving_channel = REVERSE;
};
