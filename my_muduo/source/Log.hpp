#pragma once

#include <stdio.h>
#include <ctime>

namespace muduo
{
    #define INF 0
    #define DEG 1
    #define ERR 2
    #define LOG_LEVEL DEG


    #define LOG(level, format, ...) do { \
        if (level < LOG_LEVEL) { break; } \
        time_t t = time(NULL); \
        struct tm *ltime = localtime(&t); \
        char tmp[32] = {0}; \
        strftime(tmp, 31, "%H:%M:%S", ltime); \
        fprintf(stdout, "[%s %s:%d] " format "\n", tmp, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

    #define INF_LOG(format,...) LOG(INF,format,##__VA_ARGS__)
    #define DEG_LOG(format,...) LOG(DEG,format,##__VA_ARGS__)
    #define ERR_LOG(format,...) LOG(ERR,format,##__VA_ARGS__)

}