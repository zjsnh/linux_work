#pragma once
#include<iostream>
using namespace std;

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#include "log.hpp" //日志打印

#define proj_id 0x1221


const char *PATH = "/home/zws/linux_work";

log _log;

key_t Get_key()
{
    key_t key = ftok(PATH, proj_id);
    if(key < 0)
    {
        _log(Fatal, "key create fail %d : %s", errno, strerror(errno));
        exit(errno);
    }
    return key;
}


int Create_msgQu(int flag)
{
    int msgid = msgget(Get_key(), flag);
    if(msgid < 0)
    {
        _log(Fatal, "msgid create fail %d : %s", errno, strerror(errno));
        exit(errno);
    }

    return msgid;
}

int Create_msgid()
{
    return Create_msgQu(IPC_CREAT | IPC_EXCL | 0666);  //创建
}


int Get_msgid()
{
    return Create_msgQu(IPC_CREAT | 0666); //没有就创建，存在就返回msgid
}