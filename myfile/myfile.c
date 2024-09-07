#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<string.h>
#include<unistd.h>


int main()
{

    //printf("stdin : %d\n",stdin->_fileno);
    //printf("stdout : %d\n",stdout->_fileno);
    //printf("stderr : %d\n",stderr->_fileno);
    umask(0);
    int fd = open("org.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);

    const char *message = "abcdef";
    write(fd,message,strlen(message));

    close(fd);
    return 0;
}
