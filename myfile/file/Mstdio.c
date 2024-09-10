#include<unistd.h>
#include"Mstdio.h" 
#include<assert.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdlib.h>

#define FILE_MODE 0666


_FILE* _fopen(const char* filename,const char* OPEN_WAY)
{
    assert(filename);
    assert(OPEN_WAY);

    int fd = -1;
    if(strcmp(OPEN_WAY,"r")==0)
    {
        fd = open(filename,O_RDONLY);
    }
    else if(strcmp(OPEN_WAY,"w")==0)
    {
        fd = open(filename,O_WRONLY|O_TRUNC|O_CREAT,FILE_MODE);
    }
    else if(strcmp(OPEN_WAY,"a")==0)
    {
        fd = open(filename,O_WRONLY|O_APPEND|O_CREAT,FILE_MODE);
    }
    else{
        return NULL;
    }

    if(fd==-1)
        return NULL;

    _FILE* fp =(_FILE*)malloc(sizeof(_FILE));
    fp->fileno=fd;

    fp->pos=0;

    return fp;

}



int _fwrite(_FILE* fp,const char* s,int len)
{//
    //判断刷新方式
    memcpy(&(fp->outbuf[fp->fileno]),s,len);
    fp->fileno+=len;
    if(fp->flag==0)
    {
        write(fp->fileno,fp->outbuf,fp->pos);
        fp->pos=0;
    }
    else if(fp->flag==1)
    {
        if(fp->outbuf[fp->pos-1]=='\n')
        {
            write(fp->fileno,fp->outbuf,fp->pos);
            fp->pos=0;
        }
    }
    else if(fp->flag==0)
    {
        if(fp->pos==1024)
        {
            write(fp->fileno,fp->outbuf,fp->pos);
            fp->pos=0;
        }
    }

    return 0;
}

void _fflush(_FILE* fp)
{
    if(fp->pos==0)
        return ;

    write(fp->fileno,fp->outbuf,fp->pos);
    fp->pos=0;
    return ;
}


void _fclose(_FILE* fp)
{
    close(fp->fileno);
    _fflush(fp);


    return ;
}
