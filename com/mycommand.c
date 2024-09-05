#include<unistd.h>
#include<stdio.h>

int main()
{
    printf("begin running pid: %d ,ppid: %d\n",getpid(),getppid());
    //execl("/usr/bin/ls","ls","-a","-l",NULL);
    
    char* const myargv[]={
     "./otherExe",
     "-a",
     "-l",
     NULL
    };

    char* const myenv[]={
    "MYPATH=/usr/bin/",
    "HELL=111",
    NULL
    };


    //execv("./otherExe",myargv);
     execle("./otherExe","otherExe","-a","-l",NULL,myenv);


    printf("1111111111、\n");

    return 0;
}



