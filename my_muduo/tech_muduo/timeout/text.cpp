#include "timewheel.hpp"
#include <sys/timerfd.h>
#include <unistd.h>

void task()
{
    std::cout << "Perform tasks" << std::endl;
}


void timetask()
{
    int timefd = timerfd_create(CLOCK_MONOTONIC, 0);
    if(timefd < 0)
    {
        return;
    }

    struct itimerspec itime;

    itime.it_value.tv_sec = 1;
    itime.it_value.tv_nsec = 0;
    itime.it_interval.tv_sec = 1;
    itime.it_interval.tv_nsec = 0;

    timerfd_settime(timefd, 0, &itime, NULL);

    //timefd 信号读取的描述符 类似于文件描述符

    std::shared_ptr<TimeWheel> ptr(new TimeWheel());

    ptr->TaskAdd(1110, 3, task);

    for (int i = 0; i < 3;i++)
    {
        uint64_t times;
        int ret = read(timefd, &times, 8);
        ptr->Refresh(1110);
        ptr->RunTimeTask();

        std::cout << "任务刷新" << std::endl;
    }

    ptr->TimerCancel(1110);

    while (true)
    {
        std::cout << "---------------------------" << std::endl;
        uint64_t times;
        int ret = read(timefd, &times, 8);
        ptr->RunTimeTask();
    }
}

int main()
{

    timetask();
    return 0;
}