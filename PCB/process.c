#include<stdio.h>
#include<unistd.h>
#include"../Colors/colors.h"

int main()
{

    int a=0;
    printf("ENTER: ");
    scanf("%d",&a);

    printf(RED"echo %d\n"NONE,a);
    
    return 0;
}
