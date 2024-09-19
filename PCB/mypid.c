#include<stdio.h>
#include<unistd.h>

int main()
{
    pid_t id = fork();

    if(id==0) //子进程
    {
        printf("%p\n",&id);
        int i = 5;
        while(i--)
        {
           // printf("我是子进程 pid : %d, ppid : %d\n",getpid(),getppid());
            sleep(1);
        }
    }
    else if(id>0)
    {
        printf("%p\n",&id);
        int i=7;
        while(i--)
        {
           // printf("我是父进程 pid: %d,ppid: %d\n",getpid(),getpid());
            sleep(1);
        }
    }
    else 
    {
        perror("进程创建失败\n");
    }

    return 0;
}
