#include<unistd.h>
#include"process_bar.h" 

int main()
{
    int cnt=10;
    while(cnt>=0)
    {
        printf("%2d\r",cnt);
        fflush(stdout);
        cnt--;
        sleep(1);
    }


    printf("\n");

    return 0;
}
