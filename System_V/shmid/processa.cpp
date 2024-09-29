#include "commt.hpp"
#include "log.hpp"

struct msg
{
    int _hours;
    char address[256];

    char Specifi_information[1024];
};

int main()
{
    int shmid = Create_Space();

    //挂载
    msg *buffer = (msg* )shmat(shmid, NULL, 0);

    //创建命名管道 -> fifo


    // send code
    while(true)
    {
        // cout << "input msg:";
        // cin >> buffer;
        // buffer[sizeof(buffer)] = '\0';
        // write(shmid, buffer, strlen(buffer) + 1);

        // -> 直接写入
        cout << "input msg:";
        fscanf(stdin, "%d %s %s", &buffer->_hours, 
                                buffer->address, 
                                buffer->Specifi_information);
    }

    //去挂载
    shmdt(buffer);

    int ctl = shmctl(shmid, IPC_RMID, NULL);
    if(ctl < 0)
    {
        _log(4, "shared_space exit fail %d:%s", errno, strerror(errno));
    }

    _log(0, "shared_space exit sucess %d:%s", errno, strerror(errno));

    return 0;
}