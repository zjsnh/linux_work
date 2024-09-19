#include<unistd.h>
#include<stdio.h>


int main()
{
    while(1)
    {
        printf("sleep\n");
        sleep(5);
    }
    return 0;
}
