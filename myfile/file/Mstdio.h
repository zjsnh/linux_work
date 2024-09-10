#pragma once 

#define ALL 0
#define Line 1
#define Full 2


typedef struct IO_FILE 
{
    int fileno;
    int pos;

    //缓冲区
    char outbuf[1024];
    //char inbuf[1024];
   int flag; //判断是什么缓冲模式 

}_FILE;

_FILE* _fopen(const char* filename,const char* OPEN_WAY);
int _fwrite(_FILE* fp,const char* s,int len);
void _fclose(_FILE* fp);
