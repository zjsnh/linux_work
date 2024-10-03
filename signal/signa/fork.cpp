#include <iostream>
using namespace std;

#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>


void handler(int signo)
{
    pid_t child_pid;
    while ((child_pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        cout << "process quit!!!" << child_pid << endl;
    }
}

int main()
{
    // struct sigaction act, oct;
    // memset(&act, 0, sizeof(act));
    // memset(&oct, 0, sizeof(oct));

    // act.sa_handler = handler;

    // sigaction(SIGCHLD, &act, &oct);

    signal(SIGCHLD, handler);

    for (int i = 0; i < 10; i++)
    {
        pid_t id = fork();
        if(id == 0)
        {
           while(true)
           {
               cout << "I am child : " << getpid() << " form :" << getppid() << endl;
               sleep(rand() % 3 + 2);
               break;
           }

           cout << "child quit" << endl;
           exit(0);
        }

        sleep(1);
    }

    while(true)
    {
        cout << "parent : " << getpid() << endl;
        sleep(1);
        //
    }

    return 0;
}
