#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdarg.h>
//#include <bits/fcntl-linux.h>  // 移除此行，避免使用内部实现头文件
#include <fcntl.h>  // 保留标准头文件
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#define SIZE 1024
#define Info 0
#define Debug 1
#define Warning 2
#define Error 3
#define Fatal 4

#define Screen 1
#define Onefile 2
#define Classfile 3

#define LogFile "log.txt"

class log
{
    public:
        log()
        {
            printMethod = Classfile;
            path = "./log/";
        }


        //void log_msg(int level, char *format,...)
        void operator()(int level, const char *format,...)
        {
            time_t _time = time(NULL);
            struct tm * ctime = localtime(&_time);
            char left_msg[SIZE];
            snprintf(left_msg, sizeof(left_msg), "[%s][%d-%d-%d %d:%d:%d]",log_level(level).c_str(), 
                    ctime->tm_year + 1900, ctime->tm_mon + 1, ctime->tm_mday,
                    ctime->tm_hour, ctime->tm_min, ctime->tm_sec);

            va_list ap;
            va_start(ap, format);
            char right_msg[SIZE];

            vsnprintf(right_msg, sizeof(right_msg), format, ap);
            va_end(ap);

            char msg[SIZE * 2];
            snprintf(msg, sizeof(msg),"%s : %s\n", left_msg, right_msg);

            //printf("%s\n", msg);

            printLog(level, msg);
            return;
        }

        string log_level(int level)
        {
            switch (level)
            {
            case Info:
                return "Info";
            case Debug:
                return "Debug";
            case Warning:
                return "Warning";
            case Error:
                return "Error";
            case Fatal:
                return "Fatal";
            default:
                return "None";
            }
        }

        //追加 w+
        void onefile(const string& logname,const string& logtxt)
        {
            int fd = open(logname.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
            if(fd < 0)
            {
                perror("log file create fail:");
                return;
            }

            int n = write(fd, logtxt.c_str(), logtxt.size());
            close(fd);
        }

        void classFile(int level,const string& logtxt)
        {
            string filename = path;
            filename += '.log';
            filename += log_level(level);
            //path.level


            onefile(filename, logtxt);
        }

        void printLog(int level, const std::string &logtxt)
        {
            // string _msg = '[' + log_level(level) + ']';
            // _msg += msg;
            switch (printMethod)
            {
            case Screen:
                std::cout << logtxt << std::endl;
                break;
            case Onefile:
                onefile(LogFile, logtxt);
                break;
            case Classfile:
                classFile(level, logtxt);
                break;
            default:
                break;
            }
        }

        ~log()
        {}

    private:
        int printMethod;
        std::string path;
};

log _log;