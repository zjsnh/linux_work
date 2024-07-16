#include<stdio.h>
#include<unistd.h>
int main()
{

    pid_t id=fork();

    while(1)
    {
        printf("process\n");
        sleep(1);
    }

    return 0;
}
