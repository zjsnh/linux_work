#include"process_bar.h"
#include<string.h>
#include<unistd.h>
#include<string>
#include<iostream>
using namespace std;


//const char* bar_lable="!@#$%^&*()";

const char* bar_lable="=";

void processbar()
{
   int i=0; 
   char bar[102];
   memset(bar,'\0',sizeof(bar));//初始化

   int len=strlen(bar_lable);
   const char* lable="|/-\\";
   while(i<=100)
   {
       printf(RED"[%-100s][%d%%][%c]\r"NONE,bar,i,lable[i%4]);
       fflush(stdout);
       char n=bar_lable[i%len];
       bar[i++]=n;
       usleep(10000);
   }

   printf("\n");
}
