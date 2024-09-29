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
    int shmid = Get_Space();
    msg *buffer = (msg *)shmat(shmid, NULL, 0);

    // send code
    while(true)
    {
        cout << "msg form processb:"<<"time: " 
        << buffer->_hours<<" address: "
        <<buffer->address<<" msg: "
        <<buffer->Specifi_information  << endl;
        sleep(1);
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