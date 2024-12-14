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



#define FIFO_FILE "log.txt"
#define TIME_OUT 5;

class Init
{
    public:
        Init()
        {
            int n = mkfifo(FIFO_FILE, 0664); //命名管道
            if(n < 0)
            {
                perror("file create fail:");
                exit(-1);
            }
        }


        ~Init()
        {
            int m = unlink(FIFO_FILE);
            if(m < 0)
            {
                perror("file close fail:");
                exit(-1);
            }
        }
};

