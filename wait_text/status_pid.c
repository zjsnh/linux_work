#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>

int main()
{

    pid_t id=fork();
    if(id==0)
    {
        int cur=5;
        while(cur)
        {
            printf("i am child PID: %d PPID: %d\n",getpid(),getppid());
            cur--;
            sleep(1);
        }
        exit(1);
    }
    else if(id>0){
        int cur=10;
        while(cur)
        {
            printf("i am parent PID: %d\n",getpid());
            cur--;
            sleep(1);
        }

        int status;

        pid_t ret=waitpid(id,&status,0);
        if(ret==id)
        {
            printf("status:%d\n",status);
            printf("status: %s\n",strerror(status));
        }

    }

    return 0;
}
