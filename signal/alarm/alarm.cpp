#include <iostream>
using namespace std;
#include <unistd.h>
#include <signal.h>


void my_signal(int sign)
{
    cout << "signal 2 " << sign << endl;
    //alarm(3);
}

int main()
{
    signal(SIGALRM, my_signal);
    alarm(3);  //计时器
    
    while(1)
    {
        cout << "process running" << endl;
        sleep(1);
    }

    return 0;
}
