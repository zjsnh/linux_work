//include<string.h>
/*#include"Mstdio.h"

//#define _file "text.txt"


int main()
{

    const char* msg = "hello mystdio\n";
    _FILE* fp = _fopen("text.txt","w");
    _fwrite(fp,msg,strlen(msg));

    _fclose(fp);

    return 0;
}*/

#include "Mstdio.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define myfile "test.txt"

int main()
{
    _FILE *fp = _fopen(myfile, "a");
    if(fp == NULL) return 1;

    const char *msg = "hello world\n";
    int cnt = 10;
    //while(cnt){
        _fwrite(fp, msg, strlen(msg));
        // fflush(fp);
 //       sleep(1);
    //    cnt--;
   // }

    _fclose(fp);

    return 0;
}
