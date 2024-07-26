#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>

void runChild()
{
    int cut=5;
    while(cut)
    {
        printf("child process PID: %d ,parent PID: %d, %d \n",getpid(),getppid(),cut);
        cut--;
        sleep(1);
    }

}

int main()
{
    for(int i=0;i<10;i++)
    {
        pid_t id=fork();
        if(id==0)
        {
            runChild();
            exit(0);

        }
         printf("child process success done\n");
    }

    sleep(10);

    for(int i=0;i<10;i++)
    {
       pid_t cur= waitpid(-1,NULL,0);
       if(cur>0)
       {
            printf("child process sucess recycle PID:%d \n",cur);
       }
    }
    sleep(5);

    return 0;
}
