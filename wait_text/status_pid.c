#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>


#define TASK_NUM 10
typedef void(*task_t)();
task_t task[TASK_NUM];

void task1()
{
    printf("图像绘制 pid: %d, ppid: %d \n",getpid(),getppid());
}

void task2()
{
    printf("信息更新 pid: %d, ppid: %d\n",getpid(),getppid());
}


void Insert(task_t t)
{
    for(int pos=0;pos<10;pos++)
    {
        if(!task[pos])
        {
            task[pos]=t;
            break;
        }
    }
}

void InitTask()
{
    for(int i=0;i<TASK_NUM;i++)
    {
        task[i]=NULL;
    }
}

void ExeTask()
{
    for(int i=0;i<10;i++)
    {
        if(!task[i])
            continue;
        task[i]();
    }
}


int main()
{

    InitTask();
    Insert(task1);   
    Insert(task2);

    for(int i=0;task[i];i++)
    {
        printf("%d\n",i);
    }

    sleep(5);
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

        int status = 0;

        while(1)
        {
            pid_t ret = waitpid(id,&status,WNOHANG);
            if(ret>0)
            {
                if(WIFEXITED(status))
                {
                    printf("该程序正常退出,退出码：%d\n",WEXITSTATUS(status));
                }
                else{
                    printf("进程异常\n");
                }
                break;
            }
            else if(ret<0)
            {
                printf("进程出异常了\n");
            }
            else{


                ExeTask();
                usleep(500000);
            }

            
            
        }

    }

    return 0;
}
