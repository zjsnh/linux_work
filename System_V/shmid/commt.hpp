#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>
#include "log.hpp"
#include <errno.h>
#include <string.h>
#include <cstddef>


//  初始化共享内存  提供函数接口

#define PATH "/home/zws/linux_work"
#define key_id 0x1221143

#define _SIZE 4096

log _log;

key_t GetKey_Space()
{
    key_t key = ftok(PATH, key_id);
    if(key < 0)
    {
        _log(3, "key  build failed %d :%s", errno,strerror(errno));
        exit(errno);
    }

    return key;
}

int Getshared_Space(int flag)
{
    key_t key = GetKey_Space();
    int shmid = shmget(key, _SIZE, flag);
    if(shmid < 0)
    {
        _log(3, "shmid build failed %d :%s", errno,strerror(errno));
        exit(errno);
    }

    return shmid;
}

int Create_Space()
{
    return Getshared_Space(IPC_CREAT | IPC_EXCL | 0666);  //如果没有存在这个内存就创建
}

int Get_Space()
{
    return Getshared_Space(IPC_CREAT | 0666);  //如果没有就返回错误
}
